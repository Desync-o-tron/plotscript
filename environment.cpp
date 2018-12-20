#include "environment.hpp"
#include <iostream>// for debugging
#include <cassert>
#include <cmath>
#include <complex>

#include "environment.hpp"
#include "semantic_error.hpp"

/*********************************************************************** 
Helper Functions
**********************************************************************/

// predicate, the number of args is nargs
bool nargs_equal(const std::vector<Expression> & args, unsigned nargs){
  return args.size() == nargs;
}

/*********************************************************************** 
Each of the functions below have the signature that corresponds to the
typedef'd Procedure function pointer.
**********************************************************************/

// the default procedure always returns an expresison of type None
Expression default_proc(const std::vector<Expression> & args){
  args.size(); // make compiler happy we used this parameter
  return Expression();
};

Expression add(const std::vector<Expression> & args){
  double result = 0;
  std::complex<double> complexResult(0., 0.);
  bool usingComplex = false;

  // check all aruments are numbers, while adding
  for( auto & a :args){
    if(a.isHeadNumber()){
      result += a.head().asNumber();      
    }
    else if (a.isHeadComplexNumber()) {
      complexResult += a.head().asComplexNumber();
      usingComplex = true;
    }
    else{
      throw SemanticError("Error in call to add, argument not a number");
    }
  }

  if (!usingComplex) {
    return Expression(result);
  }
  else {
    complexResult += result;
    return Expression(complexResult);
  }

};

Expression mul(const std::vector<Expression> & args){
 
  double result = 1;
  std::complex<double> complexResult(1., 0.);
  bool usingComplex = false;

  // check all aruments are numbers, while multiplying
  for( auto & a :args){
    if(a.isHeadNumber()){
      result *= a.head().asNumber();      
    }
    else if(a.isHeadComplexNumber()){
      usingComplex = true;
      complexResult *= a.head().asComplexNumber();
    }
    else{
      throw SemanticError("Error in call to mul, argument not a number");
    }
  }

  if (!usingComplex) {
    return Expression(result);
  }
  else {
    complexResult *= result;
    return Expression(complexResult);
  }

};

Expression power(const std::vector<Expression> & args) {

  double result = 0;
  std::complex<double> complexResult(0., 0.);
  bool usingComplex = false;

  //preconditions
  if (!nargs_equal(args,2))
  {
    throw SemanticError("Error in call to exponentiation: invalid number of arguments.");
  }
  else if(args[0].head().isNumber() && args[1].head().isNumber()){
    result = pow(args[0].head().asNumber(), args[1].head().asNumber());
  }
  else if (args[0].head().isNumber() && args[1].head().isComplexNumber()){
    usingComplex = true;
    complexResult = pow(args[0].head().asNumber(), args[1].head().asComplexNumber());
  }
  else if (args[0].head().isComplexNumber() && args[1].head().isNumber()){
    usingComplex = true;
    complexResult = pow(args[0].head().asComplexNumber(), args[1].head().asNumber());
  }
  else if (args[0].head().isComplexNumber() && args[1].head().isComplexNumber()){
    usingComplex = true;
    complexResult = pow(args[0].head().asComplexNumber(), args[1].head().asComplexNumber());
  }
  else{
    throw SemanticError("Error in call to exponentiation: invalid argument.");
  }

  if (!usingComplex) {
    return Expression(result);
  }
  else {
    return Expression(complexResult);
  }

}


Expression subneg(const std::vector<Expression> & args){

  double result = 0;
  std::complex<double> complexResult(0., 0.);
  bool usingComplex = false;

  // preconditions
  if(nargs_equal(args,1)){
    if(args[0].isHeadNumber()){
      result = -args[0].head().asNumber();
    }
    else if(args[0].isHeadComplexNumber()){
      complexResult -= args[0].head().asComplexNumber();
      usingComplex = true;
    }
    else{
      throw SemanticError("Error in call to negate: invalid argument.");
    }
  }
  else if(nargs_equal(args,2)){
    if( (args[0].isHeadNumber()) && (args[1].isHeadNumber()) ){
      result = args[0].head().asNumber() - args[1].head().asNumber();
    }
    else if( (args[0].isHeadNumber()) && (args[1].isHeadComplexNumber()) ){
      complexResult = args[0].head().asNumber() - args[1].head().asComplexNumber();
      usingComplex = true;
    }
    else if( (args[0].isHeadComplexNumber()) && (args[1].isHeadNumber()) ){
      complexResult += args[0].head().asComplexNumber();
      complexResult -=args[1].head().asNumber();
      usingComplex = true;
    }
    else if( (args[0].isHeadComplexNumber()) && (args[1].isHeadComplexNumber()) ){
      complexResult = args[0].head().asComplexNumber() - args[1].head().asComplexNumber();
      usingComplex = true;
    }
    else{      
      throw SemanticError("Error in call to subtraction: invalid argument.");
    }
  }
  else{
    throw SemanticError("Error in call to subtraction or negation: invalid number of arguments.");
  }

  if (!usingComplex) {
    return Expression(result);
  }
  else {
    return Expression(complexResult);
  }
};

Expression sqrt(const std::vector<Expression> & args){
  
  double result = 0;
  std::complex<double> complexResult(0., 0.);
  bool usingComplex = false;

  // preconditions
  if (nargs_equal(args, 1) == 1) 
  {
    if (args[0].isHeadNumber()) {
      if (args[0].head().asNumber() >= 0) {
        result = sqrt(args[0].head().asNumber());
      }
      else {
        complexResult = std::sqrt(std::complex<double>(args[0].head().asNumber(),0.));
        usingComplex = true;
      }
    }
    else if (args[0].isHeadComplexNumber()) {
      complexResult = std::sqrt(args[0].head().asComplexNumber());
      usingComplex = true;
    }
  }
  else {
    throw SemanticError("Error in call to sqrt: invalid number of arguments.");
  }
  if (!usingComplex){
    return Expression(result);
  }
  else {
    return Expression(complexResult);
  }
}

Expression div(const std::vector<Expression> & args){

  double result = 0;  
  std::complex<double> complexResult(0., 0.);
  bool usingComplex = false;

  if (nargs_equal(args, 2))
  {
    if ((args[0].isHeadNumber()) && (args[1].isHeadNumber())) {
      result = args[0].head().asNumber() / args[1].head().asNumber();
    }
    else if ((args[0].isHeadNumber()) && (args[1].isHeadComplexNumber())) {
      complexResult = args[0].head().asNumber() / args[1].head().asComplexNumber();
      usingComplex = true;
    }
    else if ((args[0].isHeadComplexNumber()) && (args[1].isHeadNumber())) {
      complexResult = args[0].head().asComplexNumber() / args[1].head().asNumber();
      usingComplex = true;
    }
    else if ((args[0].isHeadComplexNumber()) && (args[1].isHeadComplexNumber())) {
      complexResult = args[0].head().asComplexNumber() / args[1].head().asComplexNumber();
      usingComplex = true;
    }
    else {
      throw SemanticError("Error in call to division: invalid argument.");
    }
  }
  else if (nargs_equal(args, 1)) {
    if (args[0].isHeadComplexNumber()) {
      usingComplex = true;
      complexResult = 1. / args[0].head().asComplexNumber();
    }
    else if(args[0].isHeadNumber()){
      result = 1. / args[0].head().asNumber();
    }
    else{      
      throw SemanticError("Error in call to division: invalid argument.");
    }
  }
  else{
    throw SemanticError("Error in call to division: invalid number of arguments.");
  }
  if (!usingComplex) {
    return Expression(result);
  }
  else {
    return Expression(complexResult);
  }

};

Expression ln(const std::vector<Expression> & args) {

  double result = 0;

  if (nargs_equal(args, 1))
  {
    if ((args[0].isHeadNumber()))
    {
      if (args[0].head().asNumber() < 0)
      {
        throw SemanticError("Error in call to natural log: negative number.");
      }
      else{
        result = log(args[0].head().asNumber());
      }
    }
    else{
      throw SemanticError("Error in call to natural log: invalid argument.");
    }
  }
  else{
    throw SemanticError("Error in call to natural log: invalid number of arguments.");
  }
  return Expression(result);
}

Expression sin(const std::vector<Expression> & args) {
  
  double result = 0;

  if (nargs_equal(args, 1))
  {
    if ((args[0].isHeadNumber()))
    {
      result = (float)(sin(args[0].head().asNumber()));
    }
    else {
      throw SemanticError("Error in call to sin: invalid argument.");
    }
  }
  else {
    throw SemanticError("Error in call to sin: invalid number of arguments.");
  }

  return Expression(result);
}
// angle argument in radians.
Expression cos(const std::vector<Expression> & args) {

  double result = 0;

  if (nargs_equal(args, 1))
  {
    if ((args[0].isHeadNumber()))
    {
      result = (float)(cos(args[0].head().asNumber()));
    }
    else {
      throw SemanticError("Error in call to tan: invalid argument.");
    }
  }
  else {
    throw SemanticError("Error in call to tan: invalid number of arguments.");
  }

  return Expression(result);
}

//angle argument in radians.
Expression tan(const std::vector<Expression> & args) {

  double result = 0;

  if (nargs_equal(args, 1))
  {
    if ((args[0].isHeadNumber()))
    {
      result = (float)(tan(args[0].head().asNumber()));
    }
    else {
      throw SemanticError("Error in call to tan: invalid argument.");
    }
  }
  else {
    throw SemanticError("Error in call to tan: invalid number of arguments.");
  }

  return Expression(result);
}

Expression real(const std::vector<Expression> & args) {
  double result = 0;

  if (nargs_equal(args, 1))
  {
    if ((args[0].isHeadComplexNumber())){
      result = args[0].head().asComplexNumber().real();
    }
    else {
      throw SemanticError("Error in call to real: invalid argument.");
    }
  }
  else {
    throw SemanticError("Error in call to tan: invalid number of arguments.");
  }
  return Expression(result);
}

Expression imag(const std::vector<Expression> & args) {
  double result = 0;

  if (nargs_equal(args, 1))
  {
    if ((args[0].isHeadComplexNumber())){
      result = args[0].head().asComplexNumber().imag();
    }
    else {
      throw SemanticError("Error in call to imag: invalid argument.");
    }
  }
  else {
    throw SemanticError("Error in call to tan: invalid number of arguments.");
  }
  return Expression(result);
}

Expression mag(const std::vector<Expression> & args) {
  double result = 0;

  if (nargs_equal(args, 1))
  {
    if ((args[0].isHeadComplexNumber())){
      result += pow(args[0].head().asComplexNumber().imag(),2);
      result += pow(args[0].head().asComplexNumber().real(),2);
      result = sqrt(result);
    }
    else {
      throw SemanticError("Error in call to imag: invalid argument.");
    }
  }
  else {
    throw SemanticError("Error in call to tan: invalid number of arguments.");
  }
  return Expression(result);
}

Expression arg(const std::vector<Expression> & args) {
  double result = 0;

  if (nargs_equal(args, 1))
  {
    if ((args[0].isHeadComplexNumber())){
      result = std::arg(args[0].head().asComplexNumber());
    }
    else {
      throw SemanticError("Error in call to imag: invalid argument.");
    }
  }
  else {
    throw SemanticError("Error in call to tan: invalid number of arguments.");
  }
  return Expression(result);
}

Expression conj(const std::vector<Expression> & args) {
  std::complex<double> result;

  if (nargs_equal(args, 1))
  {
    if ((args[0].isHeadComplexNumber())){
      result = std::conj(args[0].head().asComplexNumber());
    }
    else {
      throw SemanticError("Error in call to imag: invalid argument.");
    }
  }
  else {
    throw SemanticError("Error in call to tan: invalid number of arguments.");
  }
  return Expression(result);
}

Expression list(const std::vector<Expression> & args) {
 
  Expression retList(Atom("list"));

  for (auto & a : args) {
    retList.appendExpression(a);
  }

  return retList;
}

Expression first(const std::vector<Expression> & args) {
  if(args.size() > 1)
    throw SemanticError("Error: more than one argument in call to first");
  else if (args[0].head().isList()){
    if (args[0].tailConstBegin() == args[0].tailConstEnd()) {  
      throw SemanticError("Error: argument to first is an empty list"); 
    }
    else {
      return *args[0].tailConstBegin();
    }
  }
  else {
    throw SemanticError("Error in call to first: Not a List"); 
  }
}

Expression rest(const std::vector<Expression> & args) {
  if(args.size() > 1)
    throw SemanticError("Error: more than one argument in call to rest");
  else if (args[0].head().isList()){
    if (args[0].tailConstBegin() == args[0].tailConstEnd()) {  
      throw SemanticError("Error: argument to rest is an empty list"); 
    }
    else {
      std::vector<Expression> subList;
      for (auto it = args[0].tailConstBegin() + 1; it < args[0].tailConstEnd(); ++it){
        subList.push_back(*it);
      }
      return Expression(list(subList));
    }
  }
  else {
    throw SemanticError("Error in call to rest: Not a List"); 
  }
}

Expression length(const std::vector<Expression> & args) {
  if (args.size() > 1) {
    throw SemanticError("Error: more than one argument in call to length");
  }
  else if (args[0].head().isList() || args[0].head().asSymbol() == "list") {
    int i = 0;
    for (auto it = args[0].tailConstBegin(); it < args[0].tailConstEnd(); ++it) {
      i++;
    }
    return Expression(Atom(i));
  }
  else {
    throw SemanticError("Error: argument to length is not a List"); 
  }
}

Expression append(const std::vector<Expression> & args) {
  if (args.size() != 2) {
    throw SemanticError("Error: incorrect number of arguments in call to append");
  }
  else if(!args[0].head().isList()) {
    throw SemanticError("Error: first argument to append not a list");
  }
  else {
    Expression expr(args[0]);
    expr.appendExpression(args[1]);
    return expr;
  }  
}

Expression join(const std::vector<Expression> & args) {
  if (args.size() < 2) {
    throw SemanticError("Error: too few arguments in call to join");
  }
  else {
    Expression expr(Atom("list"));
    for (auto a : args) {
      if (!a.head().isList()) {
        throw SemanticError("Error: argument to join not a list");
      }
      else {
        for (auto it = a.tailConstBegin(); it < a.tailConstEnd(); ++it) {
          expr.appendExpression(*it);
        }
      }
    }
    return expr;
  }
}

Expression range(const std::vector<Expression> & args) {
  if (args.size() != 3) {
    throw SemanticError("Error: wrong number arguments in call to range");
  }
  else {
    for (auto a : args) {
      if (!a.head().isNumber()) {
        throw SemanticError("Error: argument to range not a number");
      }
    }
    if (args[0].head().asNumber() >= args[1].head().asNumber()) {
      throw SemanticError("Error: begin greater than end in range");
    }
    else if (args[2].head().asNumber() <= 0) {
      throw SemanticError("Error: negative or zero increment in range");
    }
    else
    {
      double start = args[0].head().asNumber();
      double end = args[1].head().asNumber();
      double incr = args[2].head().asNumber();
      Expression expr(Atom("list"));
      for (double i = start; i <= end; i += incr) {
        expr.append(i);
      }
      //std::cout << "OI THIS LIST? " << expr.isHeadList();
      return expr;
    }
  }
}

Expression applyOnList(const std::vector<Expression> & args) {
  
  if (args.size() != 2) {
    throw SemanticError("Error: wrong number arguments in call to apply");
  }
  if (args[0].head().isNumber() || args[0].head().isComplexNumber() || args[0].head().isNone()) {
    throw SemanticError("Error: 1st argument to apply not a procedure");
  }
  //if (!(args[1].head().asSymbol() == "list")) { //TODO
  if (!args[1].head().isList() && !(args[1].head().asSymbol() == "list") ) {
    throw SemanticError("Error: 2nd argument to apply not a list");
  }

  Expression retProc(args[0]);
  //for (auto it = args[0].tailConstBegin(); it < args[0].tailConstEnd(); ++it) {
  for (auto it = (args[1].tailConstBegin()); it < (args[1].tailConstEnd()); it++) {
    retProc.appendExpression(*it);
  }
    
  return retProc;
}

Expression map(const std::vector<Expression> & args) {
  if (args.size() != 2) {
    throw SemanticError("Error: wrong number arguments in call to map");
  }
  if (args[0].head().isNumber() || args[0].head().isComplexNumber() || args[0].head().isNone()) {
    throw SemanticError("Error: 1st argument to map not a procedure");
  }
  
  if (!args[1].head().isList()) {
    throw SemanticError("Error: 2nd argument to map not a list");
  }
  Expression retExpr(Atom("list"));

  for (auto it = (args[1].tailConstBegin()); it < (args[1].tailConstEnd()); it++) {
    Expression listNode(args[0]);
    listNode.appendExpression(*it);
    retExpr.appendExpression(listNode);
  }
  return retExpr;
}

Expression set_property(const std::vector<Expression> & args) {
  if (args.size() != 3) {
    throw SemanticError("Error: wrong number arguments in call to set-property");
  }
  if (args[0].head().asSymbol()[0] != '"') {
    throw SemanticError("Error: key is not a string");
  }
  
  Expression retExpr(args[2]);
  //retExpr.pList.insert(std::pair<std::string, Expression>(args[0].head().asSymbol(), args[1]));
  retExpr.pList[args[0].head().asSymbol()] = args[1];

  return retExpr;
}

Expression get_property(const std::vector<Expression> & args) {
  if (args.size() != 2) {
    throw SemanticError("Error: wrong number arguments in call to get-property");
  }
  if (args[0].head().asSymbol()[0] != '"') {
    throw SemanticError("Error: key is not a string");
  }
  
  auto pos = args[1].pList.find(args[0].head().asSymbol());
  if (pos != args[1].pList.end()) {
    return pos->second;
  }
  else {
    return Expression();
  }

}


const double PI = std::atan2(0, -1);
const double EXP = std::exp(1);
const std::complex<double>  I(0,1);


Environment::Environment(){
  reset();
}

bool Environment::is_known(const Atom & sym) const{
  if(!sym.isSymbol()) return false;
  
  return envmap.find(sym.asSymbol()) != envmap.end();
}

bool Environment::is_exp(const Atom & sym) const{
  if(!sym.isSymbol()) return false;
  
  auto result = envmap.find(sym.asSymbol());
  return (result != envmap.end()) && (result->second.type == ExpressionType);
}

Expression Environment::get_exp(const Atom & sym) const{

  Expression exp;
  
  if(sym.isSymbol()){
    auto result = envmap.find(sym.asSymbol());
    if((result != envmap.end()) && (result->second.type == ExpressionType)){
      exp = result->second.exp;
    }
  }

  return exp;
}

void Environment::add_exp(const Atom & sym, const Expression & exp){

  if(!sym.isSymbol()){
    throw SemanticError("Attempt to add non-symbol to environment");
  }
    
  // if overwriting symbol map
  auto pos = envmap.find(sym.asSymbol());
  if( pos != envmap.end()){
    envmap.erase(pos);
  }

  envmap.emplace(sym.asSymbol(), EnvResult(ExpressionType, exp)); 
}

bool Environment::is_proc(const Atom & sym) const{
  if(!sym.isSymbol() && !sym.isList()) return false;
  
  auto result = envmap.find(sym.asSymbol());
  return (result != envmap.end()) && (result->second.type == ProcedureType);
}

Procedure Environment::get_proc(const Atom & sym) const{

  if(sym.isSymbol() || sym.isList()){
    auto result = envmap.find(sym.asSymbol());
    if((result != envmap.end()) && (result->second.type == ProcedureType)){
      return result->second.proc;
    }
  }

  return default_proc;
}

/*
Reset the environment to the default state. First remove all entries and
then re-add the default ones.
 */
void Environment::reset(){

  envmap.clear();
  
  // Built-In value of e
  envmap.emplace("e", EnvResult(ExpressionType, Expression(EXP)));

  // Built-In value of pi
  envmap.emplace("pi", EnvResult(ExpressionType, Expression(PI)));
  
  // Built-In value of i
  envmap.emplace("I", EnvResult(ExpressionType, Expression(I)));

  // Procedure: list
  envmap.emplace("list", EnvResult(ProcedureType, list));
  
  // Procedure: first
  envmap.emplace("first", EnvResult(ProcedureType, first));
  
  // Procedure: rest
  envmap.emplace("rest", EnvResult(ProcedureType, rest));
  
  // Procedure: length
  envmap.emplace("length", EnvResult(ProcedureType, length));

  // Procedure: append
  envmap.emplace("append", EnvResult(ProcedureType, append));

  // Procedure: apply
  envmap.emplace("apply", EnvResult(ProcedureType, applyOnList));// Procedure: apply
  
  // Procedure: map
  envmap.emplace("map", EnvResult(ProcedureType, map));

  // Procedure: join
  envmap.emplace("join", EnvResult(ProcedureType, join));

  // Procedure: range
  envmap.emplace("range", EnvResult(ProcedureType, range));

  envmap.emplace("set-property", EnvResult(ProcedureType, set_property));
  
  envmap.emplace("get-property", EnvResult(ProcedureType, get_property));
  
  // Procedure: add;
  envmap.emplace("+", EnvResult(ProcedureType, add)); 

  // Procedure: subneg;
  envmap.emplace("-", EnvResult(ProcedureType, subneg)); 

  // Procedure: sqrt;
  envmap.emplace("sqrt", EnvResult(ProcedureType, sqrt));

  // Procedure: mul;
  envmap.emplace("*", EnvResult(ProcedureType, mul)); 

  // Procedure: power;
  envmap.emplace("^", EnvResult(ProcedureType, power));

  // Procedure: div;
  envmap.emplace("/", EnvResult(ProcedureType, div)); 

  //Procedure: ln;
  envmap.emplace("ln", EnvResult(ProcedureType, ln));
  
  //Procedure: sin;
  envmap.emplace("sin", EnvResult(ProcedureType, sin));

  //Procedure: cos;
  envmap.emplace("cos", EnvResult(ProcedureType, cos));

  //Procedure: tan;
  envmap.emplace("tan", EnvResult(ProcedureType, tan));

  //Procedure: real;
  envmap.emplace("real", EnvResult(ProcedureType, real));
  
  //Procedure: imag;
  envmap.emplace("imag", EnvResult(ProcedureType, imag));
  
  //Procedure: mag;
  envmap.emplace("mag", EnvResult(ProcedureType, mag));
  
  //Procedure: arg;
  envmap.emplace("arg", EnvResult(ProcedureType, arg));

  //Procedure: arg;
  envmap.emplace("conj", EnvResult(ProcedureType, conj));

}



