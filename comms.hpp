//#include <csignal>
//#include <cstdlib>
#pragma once
#include "interpreter.hpp"
#include "message_queue.h"
#include <thread>

struct comms {

  Interpreter interp;

  struct expsNmsgs
  {
    Expression exp;
    std::string msg;
    bool msgPresent;
  };

  message_queue<std::string> inputMsgs;
  message_queue<expsNmsgs> outputMsgs;
  message_queue<bool> interruptQ;
  std::thread th1;
  bool threadOff = 0; // 0 for off, 1 for on


  // This global is needed for communication between the signal handler
  // and the rest of the code. This atomic integer counts the number of times
  // Cntl-C has been pressed by not reset by the REPL code.
  volatile sig_atomic_t global_status_flag = 0;

};

//extern comms coms;
