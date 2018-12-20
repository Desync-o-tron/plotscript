#include "expression.hpp"

#include <sstream>
#include <list>
#include <map>

#include "environment.hpp"
#include "semantic_error.hpp"

#include "parse.hpp"
#include "interpreter.hpp"
#include <algorithm>
#include <iostream>

Expression::Expression(){}

Expression::Expression(const Atom & a){

  m_head = a;
}

// recursive copy
Expression::Expression(const Expression & a){

  m_head = a.m_head;
  pList = a.pList;
  for(auto e : a.m_tail){
    m_tail.push_back(e);
  }
}

Expression & Expression::operator=(const Expression & a){

  // prevent self-assignment
  if(this != &a){
    m_head = a.m_head;
    m_tail.clear();
    pList = a.pList;
    for(auto e : a.m_tail){
      m_tail.push_back(e);
    } 
  }
  
  return *this;
}


Atom & Expression::head(){
  return m_head;
}

const Atom & Expression::head() const{
  return m_head;
}

bool Expression::isHeadNumber() const noexcept{
  return m_head.isNumber();
}

bool Expression::isHeadComplexNumber() const noexcept{
  return m_head.isComplexNumber();
}

bool Expression::isHeadSymbol() const noexcept{
  return m_head.isSymbol();
}

bool Expression::isHeadList() const noexcept {
  return m_head.isList();
}

bool Expression::isHeadLambda() const noexcept{
  return m_head.isLambda();
}

bool Expression::isTypePoint() const noexcept
{
  auto name = pList.find("\"object-name\"");
  if (name != pList.end()) {
    if (name->second.head().asSymbol() == "\"point\"")
        return true;
  }
  return false;
}

bool Expression::isTypeLine() const noexcept
{
  auto name = pList.find("\"object-name\"");
  if (name != pList.end()) {
    if (name->second.head().asSymbol() == "\"line\"")
      return true;
  }
  return false;
}

bool Expression::isTypeText() const noexcept
{
  auto name = pList.find("\"object-name\"");
  if (name != pList.end()) {
    if (name->second.head().asSymbol() == "\"text\"")
      return true;
  }
  return false;
}

//Expression Expression::getPointExpr()
//{
//  if (!this->isTypePoint())
//    return Expression();
//  //auto name = pList.find("\"object-name\"");
//  return pList.find("\"object-name\"")->second;
//
//}

//cool, but later.
//std::unique_ptr<Expression>  Expression::toTypePoint()
//{
//  auto name = pList.find("object-name");
//  if (name != pList.end()) {
//    if (name->second.head().asSymbol() == "\"point\"")
//    {
//      std::unique_ptr<Expression> retVal = (name->second);
//      
//      return retVal;
//    }
//  }   
//  return nullptr;
//}

void Expression::append(const Atom & a){
  m_tail.emplace_back(a);
}

void Expression::appendExpression(const Expression & a){
  m_tail.push_back(a);
}

Expression * Expression::tail(){
  Expression * ptr = nullptr;
  
  if(m_tail.size() > 0){
    ptr = &m_tail.back();
  }

  return ptr;
}

Expression::ConstIteratorType Expression::tailConstBegin() const noexcept{
  return m_tail.cbegin();
}

Expression::ConstIteratorType Expression::tailConstEnd() const noexcept{
  return m_tail.cend();
}

Expression apply(const Atom & op, const std::vector<Expression> & args, const Environment & env){ 
  
  // head must be a symbol or a list
  if(!op.isSymbol() && !op.isList()){
    throw SemanticError("Error during evaluation: procedure name not symbol");
  }
  
  // must map to a proc
  if (!env.is_proc(op)) {
    throw SemanticError("Error during evaluation: symbol does not name a procedure or lambda function");
  }
  
  // map from symbol to proc
  Procedure proc = env.get_proc(op);
  
  // call proc with args
  return proc(args);
}

Expression Expression::handle_lookup(const Atom & head, const Environment & env){
    if(head.isSymbol()){ // if symbol is in env return value
      if(env.is_exp(head)){
	      return env.get_exp(head);
      }
      else if (head.asSymbol() == "list") {
        return Expression(Atom("list"));
      }
      else {
	      throw SemanticError("Error during evaluation: unknown symbol");
      }
    }
    else if(head.isNumber() || head.isList() || head.isLambda()){
      return Expression(head);
    }
    else{
      throw SemanticError("Error during evaluation: Invalid type in terminal expression");
    }
}

Expression Expression::handle_begin(Environment & env){
  
  if(m_tail.size() == 0){
    throw SemanticError("Error during evaluation: zero arguments to begin");
  }

  // evaluate each arg from tail, return the last
  Expression result;
  for(Expression::IteratorType it = m_tail.begin(); it != m_tail.end(); ++it){
    result = it->eval(env);
  }
  
  return result;
}

Expression Expression::handle_define(Environment & env){

  // tail must have size 2 or error
  if(m_tail.size() != 2){
    throw SemanticError("Error during evaluation: invalid number of arguments to define");
  }
  
  // tail[0] must be symbol
  if(!m_tail[0].isHeadSymbol()){
    throw SemanticError("Error during evaluation: first argument to define not symbol");
  }

  // but tail[0] must not be a special-form or procedure
  std::string s = m_tail[0].head().asSymbol();
  if((s == "define") || (s == "begin" || (s == "lambda"))){
    throw SemanticError("Error during evaluation: attempt to redefine a special-form");
  }

  if(env.is_proc(m_head)){
    throw SemanticError("Error during evaluation: attempt to redefine a built-in procedure");
  }
  
  // eval tail[1]
  Expression result = m_tail[1].eval(env);

  if(env.is_exp(m_head)){
    throw SemanticError("Error during evaluation: attempt to redefine a previously defined symbol");
  }

  //and add to env
  env.add_exp(m_tail[0].head(), result);
  
  return result;
}

Expression Expression::handle_lambda()
{

  // tail must have size 2 or error
  if (m_tail.size() != 2) {
    throw SemanticError("Error during evaluation: invalid number of arguments to lambda");
  }
  // tail[0] must be symbol
  if (!m_tail[0].isHeadSymbol()) {
    throw SemanticError("Error during evaluation: first input argument to lambda not symbol");
  }
  // but tail[0] must not be a special-form or procedure
  std::string s = m_tail[0].head().asSymbol();
  if ((s == "define") || (s == "begin") || (s == "lambda")) {
    throw SemanticError("Error during evaluation: attempt to redefine a special-form");
  }
  //if (env.is_proc(m_head)) {
  //  throw SemanticError("Error during evaluation: attempt to redefine a built-in procedure");
  //}
  // turn inputs into an expression of type list
  std::vector<Expression> inputs;
  Expression leftBranch = this->m_tail[0];
  inputs.emplace_back(Expression(leftBranch.head()));
  for (auto e = leftBranch.tailConstBegin(); e != leftBranch.tailConstEnd(); ++e) {
    if (!(*e).isHeadSymbol()){
      throw SemanticError("Error during evaluation: input argument to lambda not symbol");
    }
    inputs.emplace_back(*e);
  }
  Expression input = list(inputs);
  // update lambda expression to have input args stored in list kind
  this->m_tail[0] = input;
  Expression funcToStore(*this);

  //std::map< std::string, Environment > localEnvs;
  //Environment localEnv(env);
  return funcToStore;
}

void find_max_min(double & max, double & min, std::vector<double> & vec) {
  max = *max_element(std::begin(vec), std::end(vec));
  min = *min_element(std::begin(vec), std::end(vec));
}
void point_grabber(std::vector<double> & xpts, std::vector<double> & ypts, std::vector<Expression> & list) {

  for (auto exp : list) {
    int i = 0;
    for (auto it = exp.tailConstBegin(); it < exp.tailConstEnd(); ++it) { ++i; }
    if (i != 2){ throw SemanticError("Error: bad point given in discrete plot"); }
    
    if(!exp.tailConstBegin()->head().isNumber())
      throw SemanticError("Error: bad point given in discrete plot");
    if(!(exp.tailConstEnd()-1)->head().isNumber())
      throw SemanticError("Error: bad point given in discrete plot");
  
    double x = exp.tailConstBegin()->head().asNumber();
    double y = (exp.tailConstEnd()-1)->head().asNumber();
    xpts.push_back(x);
    ypts.push_back(y);
  }
}
double M = 50.00;
double ITERs = 10;
double N = 20; //bounding box h/w
double A = 3;
double B = 3;
double C = 2; // vert dist from bounding box to number labels
double D = 2; // horiz dist from bounding box to number labels
double P = 0.5;

template <typename T>
std::string to_pstr(const T a_value, const int n = 4) //TODO n = 2 dude.
{
  //from stackoverflow.com/a/16606128/3894291
  std::ostringstream out;
  out.precision(n);
  //out << std::fixed << a_value;
  out << a_value;
  return out.str();
}

std::string boundingBoxCreator(std::vector<double> xpts, std::vector<double> ypts)
{
  using namespace std;
  double pymax, pymin, pxmax, pxmin; //pseudo "relative".. also y is negativeized
  find_max_min(pxmax, pxmin, xpts);
  find_max_min(pymax, pymin, ypts);
  string tlp = "(make-point " + to_pstr(pxmin) + " " + to_pstr(pymax) + ") "; //top left point
  string trp = "(make-point " + to_pstr(pxmax) + " " + to_pstr(pymax) + ") ";
  string blp = "(make-point " + to_pstr(pxmin) + " " + to_pstr(pymin) + ") ";
  string brp = "(make-point " + to_pstr(pxmax) + " " + to_pstr(pymin) + ") ";
  string boxLines = "(set-property \"thickness\" 0 (make-line " + tlp + trp + ")) ";
  boxLines += "(set-property \"thickness\" 0 (make-line " + trp + brp + ")) ";
  boxLines += "(set-property \"thickness\" 0 (make-line " + blp + brp + ")) ";
  boxLines += "(set-property \"thickness\" 0 (make-line " + tlp + blp + ")) ";
  return boxLines;
}

std::string tickPointNumberGenerator(double pxmax,double pymax, double pxmin, double  pymin, double  xmax, double ymax, double xmin, double ymin)
{
  std::string axisNums = "(set-property \"position\" (make-point " + to_pstr(pxmin - D) + " " + to_pstr(pymin) + ") " + "(make-text \"" + to_pstr(ymax, 2) + "\") ) ";
  axisNums += "(set-property \"position\" (make-point " + to_pstr(pxmin - D) + " " + to_pstr(pymax) + ") " + "(make-text \"" + to_pstr(ymin, 2) + "\") ) ";
  axisNums += "(set-property \"position\" (make-point " + to_pstr(pxmin) + " " + to_pstr(pymax + C) + ") " + "(make-text \"" + to_pstr(xmin, 2) + "\") ) ";
  axisNums += "(set-property \"position\" (make-point " + to_pstr(pxmax) + " " + to_pstr(pymax + C) + ") " + "(make-text \"" + to_pstr(xmax, 2) + "\") ) ";
  return axisNums;
}

Expression Expression::handle_dPlot(Environment & env) 
{
  using namespace std;
  double ymax, ymin, xmax, xmin; 
  double pymax, pymin, pxmax, pxmin; //pseudo "relative".. also y is negativeized
  std::vector<double> xpts;
  std::vector<double> ypts;
  vector<string> lollipopPtsVec;
  Expression discretePlotList;
  string list = "( list ";
  string pointList = " ";
  string axisNums = " ";
  string axisLines = " ";
  string lollipopLines = " ";
  string options = " ";
  enum X_axis_pos { below, inside, above };
  X_axis_pos x_axis_pos = inside;
  
  //add points
  point_grabber(xpts, ypts, m_tail[0].m_tail);
  find_max_min(xmax, xmin, xpts);
  find_max_min(ymax, ymin, ypts);
  for (size_t m = 0; m < xpts.size(); ++m) {
    xpts[m] *= N / (xmax - xmin);
  }
  for (size_t m = 0; m < ypts.size(); ++m) {
    ypts[m] *= -N / (ymax - ymin);
  }
  for (size_t i = 0; i < xpts.size(); ++i) {
    lollipopPtsVec.push_back("(set-property \"size\" " + to_pstr(P) + " (make-point " + to_pstr(xpts[i]) + " " + to_pstr(ypts[i]) + " )) ");
    pointList += "(set-property \"size\" " + to_pstr(P) + " (make-point " + to_pstr(xpts[i]) + " " + to_pstr(ypts[i]) + " )) ";
  }
  list += pointList;

  find_max_min(pxmax, pxmin, xpts);
  find_max_min(pymax, pymin, ypts);
  
  //add bounding box
  list += boundingBoxCreator(xpts, ypts);

  //cout << "\n pymax: " << pymax << " \t pymin: " << pymin << "\t diff " << pymin-pymax;
  //cout << "\n pxmax: " << pxmax << " \t pxmin: " << pxmin << "\t diff " << pxmin-pxmax;
  
  //add axies
  if (ymax >= 0 && ymin <= 0){
    string lp = "(make-point " + to_pstr(pxmin) + " " + to_pstr(0) + ") ";
    string rp = "(make-point " + to_pstr(pxmax) + " " + to_pstr(0) + ") ";
    axisLines += "(set-property \"thickness\" 0 ( make-line " + rp + lp + " )) ";
  }
  else if (ymax < 0) { x_axis_pos = above; }
  else if (ymin > 0) { x_axis_pos = below; }
  //cout << "\n ymax: " << ymax << " \t ymin: " << ymin << "\n";
  if (xmax >= 0 && xmin <= 0){
    string tp = "(make-point " + to_pstr(0) + " " + to_pstr(pymin) + ") ";
    string bp = "(make-point " + to_pstr(0) + " " + to_pstr(pymax) + ") ";
    axisLines += "(set-property \"thickness\" 0 ( make-line " + tp + bp + " ))  ";
  }
  list += axisLines; 

  //make lollipops
  for (size_t i = 0; i < xpts.size(); ++i)
  {
    string point2 = " ";
    if (x_axis_pos == inside){
      point2 = "(make-point " + to_pstr(xpts[i]) + " " + to_pstr(0) + " )";
    }
    else if (x_axis_pos == above) {
      point2 = "(make-point " + to_pstr(xpts[i]) + " " + to_pstr(pymin) + " )";
    }
    else if (x_axis_pos == below) {
      point2 = "(make-point " + to_pstr(xpts[i]) + " " + to_pstr(pymax) + " )";
    }
    lollipopLines += "(set-property \"thickness\" 0 ( make-line " + lollipopPtsVec[i] + " " + point2 + " )) ";
  }
  list += lollipopLines;

  //(list
  //(list "title" "The Data")
  //(list "abscissa-label" "X Label")
  //(list "ordinate-label" "Y Label")
  //(list "text-scale" 1))))
  double scale = 1;
  for (size_t i = 0; i < m_tail[1].m_tail.size(); ++i) {
    if (m_tail[1].m_tail[i].m_tail.size() != 2) {
      throw SemanticError("Error in call to discrete-plot: bad OPTIONS parameter.");
    }
    if (m_tail[1].m_tail[i].m_tail[0].head().asSymbol() == "\"text-scale\"") {
      scale = m_tail[1].m_tail[i].m_tail[1].head().asNumber();
    }
  }
  for (size_t i = 0; i < m_tail[1].m_tail.size(); ++i) {
    if (m_tail[1].m_tail[i].m_tail.size() != 2) {
      throw SemanticError("Error in call to discrete-plot: bad OPTIONS parameter.");
    }
    if (m_tail[1].m_tail[i].m_tail[0].head().asSymbol() == "\"title\"") {
      string pos = " (make-point " + to_pstr(pxmin+(pxmax-pxmin)/2.0) + " " + to_pstr(pymin - A) + " )";
      options += "( set-property \"text-scale\" " + to_pstr(scale) + " ";
      options += "(set-property \"position\" " + pos + "(make-text " + m_tail[1].m_tail[i].m_tail[1].head().asSymbol() + ") ) )";
    }
    else if (m_tail[1].m_tail[i].m_tail[0].head().asSymbol() == "\"abscissa-label\"") {
      string pos = " (make-point " + to_pstr(pxmin + (pxmax - pxmin) / 2.0) + " " + to_pstr(pymax + A) + " )";
      options += "( set-property \"text-scale\" " + to_pstr(scale) + " ";
      options += "(set-property \"position\" " + pos + "(make-text " + m_tail[1].m_tail[i].m_tail[1].head().asSymbol() + ") ) )";
    }
    else if (m_tail[1].m_tail[i].m_tail[0].head().asSymbol() == "\"ordinate-label\"") {
      string rotation = "(set-property \"text-rotation\" (* 270 (/ pi 180 )) ";
      string pos = " (make-point " + to_pstr(pxmin - B) + " " + to_pstr(pymax-(pymax-pymin)/2.0) + " )";
      options += "( set-property \"text-scale\" " + to_pstr(scale) + " ";
      options += rotation + "(set-property \"position\" " + pos + "(make-text " + m_tail[1].m_tail[i].m_tail[1].head().asSymbol() + ") ) ) )";
    }
  }
  //cout << options;
  list += options;

  //add tick point numbers
  list += tickPointNumberGenerator(pxmax, pymax, pxmin, pymin, xmax, ymax, xmin, ymin);
    
  list += " )";
  /*cout << list;*/
  std::istringstream iss(list);
  TokenSequenceType tokens = tokenize(iss);
  auto ast = parse(tokens);
  if (ast == Expression())
    std::cerr << "Error: bad discrete plot input." << std::endl;
  else {
    try {
      discretePlotList = ast.eval(env);
    }
    catch (const SemanticError & ex) {
      std::cerr << ex.what() << std::endl;
    }
  }
  return discretePlotList;
}

std::string boundingBoxCreatorCont(std::vector<double> xpts, std::vector<double> ypts)
{
  using namespace std;
  double pymax, pymin, pxmax, pxmin; //pseudo "relative".. also y is negativeized
  find_max_min(pxmax, pxmin, xpts);
  find_max_min(pymax, pymin, ypts);
  string tlp = "(make-point " + to_pstr(pxmin) + " " + to_pstr(pymax) + ") "; //top left point
  string trp = "(make-point " + to_pstr(pxmax) + " " + to_pstr(pymax) + ") ";
  string blp = "(make-point " + to_pstr(pxmin) + " " + to_pstr(pymin) + ") ";
  string brp = "(make-point " + to_pstr(pxmax) + " " + to_pstr(pymin) + ") ";
  string boxLines = "(set-property \"thickness\" 0 (make-line " + tlp + trp + ")) ";
  boxLines += "(set-property \"thickness\" 0 (make-line " + trp + brp + ")) ";
  boxLines += "(set-property \"thickness\" 0 (make-line " + blp + brp + ")) ";
  boxLines += "(set-property \"thickness\" 0 (make-line " + tlp + blp + ")) ";
  return boxLines;
}

std::string Expression::optionsGenerator(double pxmax, double pymax, double pxmin, double pymin, int tailpos)
{
  //(list
  //(list "title" "The Data")
  //(list "abscissa-label" "X Label")
  //(list "ordinate-label" "Y Label")
  //(list "text-scale" 1))))
  using namespace std;
  string options = " ";
  double scale = 1;
  for (size_t i = 0; i < m_tail[tailpos].m_tail.size(); ++i) {
    if (m_tail[tailpos].m_tail[i].m_tail.size() != 2) {
      throw SemanticError("Error in call to plot: bad OPTIONS parameter.");
    }
    if (m_tail[tailpos].m_tail[i].m_tail[0].head().asSymbol() == "\"text-scale\"") {
      scale = m_tail[tailpos].m_tail[i].m_tail[1].head().asNumber();
    }
  }
  for (size_t i = 0; i < m_tail[tailpos].m_tail.size(); ++i) {
    if (m_tail[tailpos].m_tail[i].m_tail.size() != 2) {
      throw SemanticError("Error in call to plot: bad OPTIONS parameter.");
    }
    if (m_tail[tailpos].m_tail[i].m_tail[0].head().asSymbol() == "\"title\"") {
      string pos = " (make-point " + to_pstr(pxmin + (pxmax - pxmin) / 2.0) + " " + to_pstr(pymin - A) + " )";
      options += "( set-property \"text-scale\" " + to_pstr(scale) + " ";
      options += "(set-property \"position\" " + pos + "(make-text " + m_tail[tailpos].m_tail[i].m_tail[1].head().asSymbol() + ") ) )";
      //cout << "\n AHHHHHHHHHHHHHHHHHHHH: " << pos << endl;
    }
    else if (m_tail[tailpos].m_tail[i].m_tail[0].head().asSymbol() == "\"abscissa-label\"") {
      string pos = " (make-point " + to_pstr(pxmin + (pxmax - pxmin) / 2.0) + " " + to_pstr(pymax + A) + " )";
      options += "( set-property \"text-scale\" " + to_pstr(scale) + " ";
      options += "(set-property \"position\" " + pos + "(make-text " + m_tail[tailpos].m_tail[i].m_tail[1].head().asSymbol() + ") ) )";
    }
    else if (m_tail[tailpos].m_tail[i].m_tail[0].head().asSymbol() == "\"ordinate-label\"") {
      string rotation;
      if (tailpos == 1)
        rotation = "(set-property \"text-rotation\" (* 270 (/ pi 180 )) ";
      if (tailpos == 2)
        rotation = "(set-property \"text-rotation\" (0) ";
      string pos = " (make-point " + to_pstr(pxmin - B) + " " + to_pstr(pymax - (pymax - pymin) / 2.0) + " )";
      options += "( set-property \"text-scale\" " + to_pstr(scale) + " ";
      options += rotation + "(set-property \"position\" " + pos + "(make-text " + m_tail[tailpos].m_tail[i].m_tail[1].head().asSymbol() + ") ) ) )";
    }
  }
  return options;
}

Expression Expression::handle_cPlot(Environment & env)
{
  using namespace std;
  //m_tail[0]; //lambda function
  //m_tail[1]; //range
  //enum X_axis_pos { below, inside, above };
  //X_axis_pos x_axis_pos = inside;
  double ymax, ymin, xmax, xmin;
  double pymax, pymin, pxmax, pxmin; //pseudo "relative".. also y is negativeized
  std::vector<double> xpts;
  std::vector<double> ypts;
  Expression continuousPlotList;
  string list = "( list ";
  string pointList = " ";
  string axisLines = " ";

  auto exp = m_tail[1];
  if (!exp.tailConstBegin()->head().isNumber())
    throw SemanticError("Error: bad point given in contin plot");
  if (!(exp.tailConstEnd() - 1)->head().isNumber())
    throw SemanticError("Error: bad point given in contin plot");
  xmin = exp.tailConstBegin()->head().asNumber();
  xmax = (exp.tailConstEnd() - 1)->head().asNumber();

  double stepSize = (xmax - xmin) / M;
  //cout << stepSize << endl << endl;
  for (double i = xmin; i <= xmax + stepSize; i += stepSize) {
    //cout << i << endl;
    xpts.push_back(i);
  }
  //if (xpts.back() != xmax) //hacky? yes. Apparently, it doesn't matter...?
  //  xpts.push_back(xmax);
  cout << xpts.back() << endl;
  cout << xpts.size();

  for (auto x : xpts)
  {
    string ypoint = "( " + m_tail[0].head().asSymbol() + " " + to_pstr(x, 5) + " )";
    std::istringstream iss(ypoint);
    TokenSequenceType tokens = tokenize(iss);
    auto ast = parse(tokens);
    if (ast == Expression())
      std::cerr << "Error: bad contin plot input." << std::endl;
    else {
      try {
        ypts.push_back(ast.eval(env).head().asNumber());
        cout << "x: " <<x << " & y: ";
        cout << ypts.back() << endl;
      }
      catch (const SemanticError & ex) {
        std::cerr << ex.what() << std::endl;
      }
    }
  }
  if (ypts.size() != xpts.size()) {
    return Expression();
  }
  find_max_min(xmax, xmin, xpts);
  find_max_min(ymax, ymin, ypts);

  /////////now we've got x&y lists
  //a ^ 2 = b ^ 2 + c ^ 2 - 2bccosA
  //(1)
  //  b ^ 2 = a ^ 2 + c ^ 2 - 2accosB

  //  (2)
  //  c ^ 2 = a ^ 2 + b ^ 2 - 2abcosC.
  cout << "\n";
  
  for (int passes = 0; passes < ITERs; ++passes) {
    for (size_t i = 0; i < xpts.size() - 2; ++i) {
      double p0x = xpts[i];
      double p0y = ypts[i];
      double p1x = xpts[i+1];
      double p1y = ypts[i+1];
      double p2x = xpts[i+2];
      double p2y = ypts[i+2];
      
      double a = pow(p1x - p0x, 2) + pow(p1y - p0y, 2);
      double b = pow(p1x - p2x, 2) + pow(p1y - p2y, 2);
      double c = pow(p2x - p0x, 2) + pow(p2y - p0y, 2);
      cout << a << " " << b << " " << c << " " << sqrt(4 * a*b) << " " << acos((a + b - c)) << endl;
      //double angle = acos((a + b - c) / sqrt(4 * a*b));
      double angleTop = acos(a + b - c);
      double angleBot = sqrt(4 * a*b);
      double angle = angleTop / angleBot / 3.14159265359 * 180.0;
      //0.032 0.032 0.128 0.064 1.63484
      //double angle2 = 1.63484 / 0.064 ;
      cout << angle << endl;
    }
    break;
  }
  /*
  function findAngle(p0,p1,p2) {
  var a = Math.pow(p1.x-p0.x,2) + Math.pow(p1.y-p0.y,2),
      b = Math.pow(p1.x-p2.x,2) + Math.pow(p1.y-p2.y,2),
      c = Math.pow(p2.x-p0.x,2) + Math.pow(p2.y-p0.y,2);
  return Math.acos( (a+b-c) / Math.sqrt(4*a*b) );
}
  */


  //scaling time
  for (size_t m = 0; m < xpts.size(); ++m) {
    xpts[m] *= N / (xmax - xmin);
    ypts[m] *= -N / (ymax - ymin);
  }
  find_max_min(pxmax, pxmin, xpts);
  find_max_min(pymax, pymin, ypts);
  //cout << "\n pymax: " << pymax << " \t pymin: " << pymin << "\t diff " << pymin-pymax;
  //cout << "\n pxmax: " << pxmax << " \t pxmin: " << pxmin << "\t diff " << pxmin-pxmax;

  for (size_t i = 0; i < xpts.size() - 1; ++i)
  {
    string point1 = "(make-point " + to_pstr(xpts[i], 5) + " " + to_pstr(ypts[i], 5) + " )";
    string point2 = "(make-point " + to_pstr(xpts[i + 1], 5) + " " + to_pstr(ypts[i + 1], 5) + " )";
    list += "(set-property \"thickness\" 0 (make-line " + point1 + " " + point2 + " ) )";
  }

  list += boundingBoxCreatorCont(xpts, ypts);
  //list += axisLineGenerator(xmax, ymax, xmin, ymin, pxmax, pymax, pxmin, pymin);
  if (ymax >= 0 && ymin <= 0) {
    string lp = "(make-point " + to_pstr(pxmin) + " " + to_pstr(0) + ") ";
    string rp = "(make-point " + to_pstr(pxmax) + " " + to_pstr(0) + ") ";
    axisLines += "(set-property \"thickness\" 0 ( make-line " + rp + lp + " )) ";
  }
  //else if (ymax < 0) { x_axis_pos = above; }
  //else if (ymin > 0) { x_axis_pos = below; }
  //cout << "\n ymax: " << ymax << " \t ymin: " << ymin << "\n";
  if (xmax >= 0 && xmin <= 0) {
    string tp = "(make-point " + to_pstr(0) + " " + to_pstr(pymin) + ") ";
    string bp = "(make-point " + to_pstr(0) + " " + to_pstr(pymax) + ") ";
    axisLines += "(set-property \"thickness\" 0 ( make-line " + tp + bp + " ))  ";
  }
  list += axisLines;

  list += tickPointNumberGenerator(pxmax, pymax, pxmin, pymin, xmax, ymax, xmin, ymin);
  if (m_tail.size() == 3) {
    list += optionsGenerator(pxmax, pymax, pxmin, pymin, 2);
  }
  list += ")";
  std::istringstream iss(list);
  TokenSequenceType tokens = tokenize(iss);
  auto ast = parse(tokens);
  if (ast == Expression())
    std::cerr << "Error: bad contin plot input." << std::endl;
  else {
    try {
      continuousPlotList = ast.eval(env);
    }
    catch (const SemanticError & ex) {
      std::cerr << ex.what() << std::endl;
    }
  }
  return continuousPlotList;
}

// this is a simple recursive version. the iterative version is more
// difficult with the ast data structure used (no parent pointer).
// this limits the practical depth of our AST
Expression Expression::eval(Environment & env){

  bool interruptFlag = 0;
  if(!env.testing)
    if (env.interruptQ->try_pop(interruptFlag))  {
      throw SemanticError("Error: interpreter kernel interrupted");
    }

  if (this->head().asSymbol() == "apply") {
    Expression expr(applyOnList(m_tail));
    return expr.eval(env);
  }
  if (this->head().asSymbol() == "map") {
    Expression list = m_tail[1];
    m_tail[1] = list.eval(env);
    Expression expr(map(m_tail));
    return expr.eval(env);
  }
  if (this->head().asSymbol() == "discrete-plot") {
    if(m_tail.size() != 2)
      throw SemanticError("Error in call to discrete-plot: invalid number of lists.");
    Expression list = m_tail[0];
    Expression list2 = m_tail[1];
    m_tail[0] = list.eval(env);
    m_tail[1] = list2.eval(env);
    return handle_dPlot(env);
  }
  if (this->head().asSymbol() == "continuous-plot") {
    if (m_tail.size() != 2 && m_tail.size() != 3)
      throw SemanticError("Error in call to continuous-plot: invalid number of inputs. You have: " + to_pstr(m_tail.size()) + " inputs.");
    Expression list2 = m_tail[1]; //bounds
    if (m_tail.size() == 3) {
      Expression list3 = m_tail[2]; //options
      m_tail[2] = list3.eval(env);
    }
    m_tail[1] = list2.eval(env);
    return handle_cPlot(env);
  }
  
  if(m_tail.empty()){
    if (this->head().asSymbol()[0] == '"' && this->head().asSymbol()[this->head().asSymbol().size()-1] == '"')
      return *this;
    return handle_lookup(m_head, env);
  }
  // handle begin special-form
  else if(m_head.isSymbol() && m_head.asSymbol() == "begin"){
    return handle_begin(env);
  }
  // handle define special-form
  else if(m_head.isSymbol() && m_head.asSymbol() == "define"){
    return handle_define(env);
  }
  // handle lambda special-form
  else if(m_head.isLambda() && m_head.asSymbol() == "lambda"){ 
    return handle_lambda();
  }
  // else attempt to treat as procedure
  else{ 
    std::vector<Expression> results;
    for(Expression::IteratorType it = m_tail.begin(); it != m_tail.end(); ++it){
      results.push_back(it->eval(env));
    }
    if (env.get_exp(m_head).isHeadLambda()) {
      Environment localEnv = localEnvMaker(env);
      return callALambda(m_head, results, localEnv);
      //return callALambda(m_head, results, env);
    }
    else {
      return apply(m_head, results, env);
    }
  }
}

Environment localEnvMaker(Environment env) {
  //deep copy env
  Environment localEnv(env);
  return localEnv;
}


Expression callALambda(const Atom & op, const std::vector<Expression> & args, Environment & env) {
  //TODO..try and break?

  //we're dealing with lambda funct
  Expression lambdafunc = env.get_exp(op);
  //then turn args into map variables
  auto inputList = lambdafunc.tailConstBegin();
  size_t inputSize = 0;
  for (auto e = (*inputList).tailConstBegin(); e != (*inputList).tailConstEnd(); ++e) {
    inputSize++;
  }
  if (args.size() == inputSize) {
    int i = 0;
    for (auto e = (*inputList).tailConstBegin(); e != (*inputList).tailConstEnd(); ++e) {
      env.add_exp((*e).head(), args[i]);
      i++;
    }
  }
  else {
    throw SemanticError("Error in call to function: invalid number of arguments.");
  }
  //then pull the expression branch from the tail and send it to eval
  auto functBody = lambdafunc.tailConstBegin() + 1;
  Expression returnExpr(*functBody);
  return returnExpr.eval(env); 
}


std::ostream & operator<<(std::ostream & out, const Expression & exp){

  if (exp.head().isNone()) {
    out << "NONE";
    return out;
  }

  out << "(";
  out << exp.head();

  int i = 0;
  for(auto e = exp.tailConstBegin(); e != exp.tailConstEnd(); ++e){
    
    if (i == 0 && !exp.head().isList() && !exp.head().isLambda())
    {
      out << " ";
      ++i;
    }
    if (e+1 == exp.tailConstEnd()) {
      out << *e ;
    }
    else {
      out << *e << " ";
    }

  }

  out << ")";

  return out;
}

bool Expression::operator==(const Expression & exp) const noexcept{

  bool result = (m_head == exp.m_head);

  result = result && (m_tail.size() == exp.m_tail.size());

  if(result){
    for(auto lefte = m_tail.begin(), righte = exp.m_tail.begin();
	(lefte != m_tail.end()) && (righte != exp.m_tail.end());
	++lefte, ++righte){
      result = result && (*lefte == *righte);
    }
  }

  return result;
}

bool operator!=(const Expression & left, const Expression & right) noexcept{

  return !(left == right);
}
