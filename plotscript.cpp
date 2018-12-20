#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <thread>

#include "interpreter.hpp"
#include "semantic_error.hpp"
#include "startup_config.hpp"
#include "message_queue.h"
#include <csignal>
#include <cstdlib>

using namespace std;

// This global is needed for communication between the signal handler
// and the rest of the code. This atomic integer counts the number of times
// Cntl-C has been pressed by not reset by the REPL code.
volatile sig_atomic_t global_status_flag = 0;
message_queue<bool> * interruptQ;

// *****************************************************************************
// install a signal handler for Cntl-C on Windows
// *****************************************************************************
#if defined(_WIN64) || defined(_WIN32)
#include <windows.h>

// this function is called when a signal is sent to the process
BOOL WINAPI interrupt_handler(DWORD fdwCtrlType) {

  switch (fdwCtrlType) {
  case CTRL_C_EVENT: // handle Cnrtl-C
    // if not reset since last call, exit
    if (global_status_flag > 0) {
      exit(EXIT_FAILURE);
    }
    ++global_status_flag;
    interruptQ->push(bool(true));
    return TRUE;

  default:
    return FALSE;
  }
}


// install the signal handler
inline void install_handler() { SetConsoleCtrlHandler(interrupt_handler, TRUE); }
// *****************************************************************************

// *****************************************************************************
// install a signal handler for Cntl-C on Unix/Posix
// *****************************************************************************
#elif defined(__APPLE__) || defined(__linux) || defined(__unix) ||             \
    defined(__posix)
#include <unistd.h>

// this function is called when a signal is sent to the process
void interrupt_handler(int signal_num) {

  if (signal_num == SIGINT) { // handle Cnrtl-C
    // if not reset since last call, exit
    if (global_status_flag > 0) {
      exit(EXIT_FAILURE);
    }
    ++global_status_flag;
    interruptQ->push(bool(true));
  }
}

// install the signal handler
inline void install_handler() {

  struct sigaction sigIntHandler;

  sigIntHandler.sa_handler = interrupt_handler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;

  sigaction(SIGINT, &sigIntHandler, NULL);
}
#endif
// *****************************************************************************

struct expsNmsgs
{
  Expression exp;
  std::string msg;
  bool msgPresent;
};

void threaded_interp(message_queue<std::string> * inputMsgs, message_queue<expsNmsgs> * outputMsgs, Interpreter * interp);
void repl(message_queue<std::string> * inputMsgs, message_queue<expsNmsgs> * outputMsgs, Interpreter * interp, thread & th1, bool threadOff);

void prompt() {
  std::cout << "\nplotscript> ";
}

std::string readline() {
  std::string line;
  std::getline(std::cin, line);

  return line;
}

void error(const std::string & err_str) {
  std::cerr << "Error: " << err_str << std::endl;
}

void info(const std::string & err_str) {
  std::cout << "Info: " << err_str << std::endl;
}

int eval_from_stream(std::istream & stream, std::string filename, message_queue<std::string> * inputMsgs, message_queue<expsNmsgs> * outputMsgs, Interpreter * interp, thread & th1, bool threadOff) {

  if (!interp->parseStream(stream)) {
    error("Invalid Program. Could not parse.");
    //return EXIT_FAILURE;
  }
  else {
    try {
      Expression exp = interp->evaluate(interruptQ, false);
      std::cout << exp << std::endl;
    }
    catch (const SemanticError & ex) {
      std::cerr << ex.what() << std::endl;
      //return EXIT_FAILURE;
    }
  }

  //return EXIT_SUCCESS;
  if (filename == STARTUP_FILE)
    repl(inputMsgs, outputMsgs, interp, th1, threadOff);
  return 0;
}

int eval_from_file(std::string filename, message_queue<std::string> * inputMsgs, message_queue<expsNmsgs> * outputMsgs, Interpreter * interp, thread & th1, bool threadOff) {

  std::ifstream ifs(filename);

  if (!ifs) {
    error("Could not open file for reading.");
    return EXIT_FAILURE;
  }

  return eval_from_stream(ifs, filename, inputMsgs, outputMsgs, interp, th1, threadOff);
}

int eval_from_command(std::string argexp, message_queue<std::string> * inputMsgs, message_queue<expsNmsgs> * outputMsgs, Interpreter * interp, thread & th1, bool threadOff) {

  std::istringstream expression(argexp);

  return eval_from_stream(expression, argexp, inputMsgs, outputMsgs, interp, th1, threadOff);
}


int main(int argc, char *argv[])
{
  message_queue<std::string> * inputMsgs = new message_queue<std::string>();
  message_queue<expsNmsgs> * outputMsgs = new message_queue<expsNmsgs>();
  interruptQ = new message_queue<bool>();
  Interpreter * interp = new Interpreter();
  std::thread th1(threaded_interp, inputMsgs, outputMsgs, interp);
  bool threadOff = 0;

  install_handler();

  if (argc == 2) {
    // return eval_from_file(argv[1]);
    if (eval_from_file(argv[1], inputMsgs, outputMsgs, interp, th1, threadOff) == EXIT_FAILURE)
      goto end;
  }
  else if (argc == 3) {
    if (std::string(argv[1]) == "-e") {
      //return eval_from_command(argv[2]);
      if (eval_from_command(argv[2], inputMsgs, outputMsgs, interp, th1, threadOff) == EXIT_FAILURE)
        goto end;
    }
    else {
      error("Incorrect number of command line arguments.");
    }
  }
  else {
    eval_from_file(STARTUP_FILE, inputMsgs, outputMsgs, interp, th1, threadOff);
  }

end:
  if (!threadOff)
  {
    inputMsgs->push("EXIT_LOOP_");
    th1.join();
  }
  delete interp;
  delete inputMsgs;
  delete outputMsgs;
  delete interruptQ;

  return EXIT_SUCCESS;
}

void threaded_interp(message_queue<std::string> * inputMsgs, message_queue<expsNmsgs> * outputMsgs, Interpreter * interp)
{
  std::string msg;
  expsNmsgs outputMsg;
  while (1)
  {
    bool x;
    while (interruptQ->try_pop(x))
      x = 0;

    if (inputMsgs->try_pop(msg))
    {
      if (msg == "EXIT_LOOP_")
        return;

      std::istringstream expression(msg);

      if (!interp->parseStream(expression)) {
        //error("Invalid Expression. Could not parse.");
        //exp = Expression(Atom("Invalid Expression. Could not parse."));
        outputMsg.msg = "Invalid Expression. Could not parse.";
        outputMsg.msgPresent = 1;
      }
      else {
        try {
          outputMsg.exp = interp->evaluate(interruptQ,false);
          outputMsg.msgPresent = 0;
        }
        catch (const SemanticError & ex) {
          //std::cerr << ex.what() << std::endl;
          //exp = Expression(Atom(ex.what()));
          outputMsg.msg = ex.what();
          outputMsg.msgPresent = 1;
        }
      }
      outputMsgs->push(outputMsg);
    }
  }
}

// A REPL is a repeated read-eval-print loop
void repl(message_queue<std::string> * inputMsgs, message_queue<expsNmsgs> * outputMsgs, Interpreter * interp, thread & th1, bool threadOff) {

  while (1) {
    global_status_flag = 0;
    //bool x;
    //while (interruptQ->try_pop(x))
    //  x = 0;

    if (std::cin.eof() && !threadOff) {
      std::cin.clear(); // reset cin state
      error("interpreter kernel interrupted");
    }

    prompt();
    std::string line = readline();
    if (line.empty()) continue;

    if (line == "%start") {
      if (threadOff)
        th1 = thread(threaded_interp, inputMsgs, outputMsgs, interp);
      threadOff = 0;
    }
    else if (line == "%stop") {
      if (!threadOff) {
        inputMsgs->push("EXIT_LOOP_");
        th1.join();
        threadOff = 1;
      }
    }
    else if (line == "%reset") {
      if (threadOff)
        th1 = thread(threaded_interp, inputMsgs, outputMsgs, interp);
      delete interp;
      interp = new Interpreter();
      threadOff = 0;
    }
    else
    {
      if (threadOff)
        error("Error: interpreter kernel not running");
      else
      {
        inputMsgs->push(line);
        expsNmsgs msg;
        outputMsgs->wait_and_pop(msg);
        if (!msg.msgPresent)
          std::cout << msg.exp << std::endl;
        else
          std::cout << msg.msg << std::endl;
      }
    }
  }
}