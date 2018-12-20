#include "interpreter.hpp"

// system includes
#include <stdexcept>

// module includes
#include "token.hpp"
#include "parse.hpp"
#include "expression.hpp"
#include "environment.hpp"
#include "semantic_error.hpp"
#include "message_queue.h"

bool Interpreter::parseStream(std::istream & expression) noexcept{

  TokenSequenceType tokens = tokenize(expression);

  ast = parse(tokens);

  return (ast != Expression());
};
				     

Expression Interpreter::evaluate(message_queue<bool> * interruptQ, bool testing){
  env.testing = testing;
  env.interruptQ = interruptQ;
  return ast.eval(env);
}

