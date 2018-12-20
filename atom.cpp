#include "atom.hpp"

#include <sstream>
#include <cctype>
#include <cmath>
#include <limits>

Atom::Atom(): m_type(NoneKind) {}

Atom::Atom(double value){
  setNumber(value);
}

Atom::Atom(std::complex<double> value){
  setComplexNumber(value);
}

Atom::Atom(const Token & token): Atom(){
  // is token a number?
  double temp;
  std::istringstream iss(token.asString());
  if(iss >> temp){
    // check for trailing characters if >> succeeds
    if(iss.rdbuf()->in_avail() == 0){
      setNumber(temp);
    }
  }
  else{ // else assume symbol or list or lambda
    if (token.asString() == "list"){
      setList();
    }
    if (token.asString() == "lambda"){
      setLambda();
    }
    // make sure does not start with number
    else if(!std::isdigit(token.asString()[0])){
      setSymbol(token.asString());
    }
  }
}

Atom::Atom(const std::string & value): Atom() {
  
  if (value == "list") {
    setList();
  }
  else if (value == "lambda") {
    setLambda();
  }
  else {
    setSymbol(value);
  }
}

Atom::Atom(const Atom & x): Atom(){
  if(x.isNumber()){
    setNumber(x.numberValue);
  }
  else if(x.isComplexNumber()){
    setComplexNumber(x.complexNumberValue);
  }
  else if(x.isSymbol()){
    setSymbol(x.stringValue);
  }
  else if(x.isList()){
    setList();
  }
  else if(x.isLambda()){
    setLambda();
  }
}

Atom & Atom::operator=(const Atom & x){

  if(this != &x){
    if(x.m_type == NoneKind){
      m_type = NoneKind;
    }
    else if(x.m_type == NumberKind){
      setNumber(x.numberValue);
    }
    else if (x.m_type == ComplexNumberKind) {
      setComplexNumber(x.complexNumberValue);
    }
    else if(x.m_type == SymbolKind){
      setSymbol(x.stringValue);
    }
    else if (x.m_type == ListKind) {
      setList();
    }
    else if (x.m_type == LambdaKind) {
      setLambda();
    }
  }
  return *this;
}
  
Atom::~Atom(){
  // we need to ensure the destructor of the symbol string is called
  if(m_type == SymbolKind || m_type == ListKind || m_type == LambdaKind){
    stringValue.~basic_string();
  }
}

bool Atom::isNone() const noexcept{
  return m_type == NoneKind;
}

bool Atom::isNumber() const noexcept{
  return m_type == NumberKind;
}

bool Atom::isList() const noexcept{
  return m_type == ListKind;
}

bool Atom::isLambda() const noexcept{
  return m_type == LambdaKind;
}

bool Atom::isComplexNumber() const noexcept{
  return m_type == ComplexNumberKind;
}

bool Atom::isSymbol() const noexcept{
  return m_type == SymbolKind;
}  

// --------setters---------

void Atom::setNumber(double value){

  m_type = NumberKind;
  numberValue = value;
}

void Atom::setComplexNumber(std::complex<double> value){

  m_type = ComplexNumberKind;
  complexNumberValue = value;
}

void Atom::setSymbol(const std::string & value){

  // we need to ensure the destructor of the symbol string is called
  if(m_type == SymbolKind || m_type == ListKind || m_type == LambdaKind){
    stringValue.~basic_string();
  }
    
  m_type = SymbolKind;

  // copy construct in place
  new (&stringValue) std::string(value);
}

void Atom::setList()
{
  // we need to ensure the destructor of the symbol string is called
  if (m_type == SymbolKind || m_type == ListKind || m_type == LambdaKind) {
    stringValue.~basic_string();
  }

  m_type = ListKind;

  // copy construct in place
  new (&stringValue) std::string("list");
}

void Atom::setLambda()
{
  // we need to ensure the destructor of the symbol string is called
  if (m_type == SymbolKind || m_type == ListKind || m_type == LambdaKind) {
    stringValue.~basic_string();
  }

  m_type = LambdaKind;

  // copy construct in place
  new (&stringValue) std::string("lambda");
}


double Atom::asNumber() const noexcept{

  return (m_type == NumberKind) ? numberValue : 0.0;  
}

std::complex<double> Atom::asComplexNumber() const noexcept{

  return (m_type == ComplexNumberKind) ? complexNumberValue : std::complex<double>(0.,0.);
}

std::string Atom::asSymbol() const noexcept{

  std::string result;

  if(m_type == SymbolKind || m_type == ListKind || m_type == LambdaKind){
    result = stringValue;
  }
 
  return result;
}


bool Atom::operator==(const Atom & right) const noexcept{
  
  if(m_type != right.m_type) return false;

  switch(m_type){
  case NoneKind:
    if(right.m_type != NoneKind) return false;
    break;
  case NumberKind:
    {
      if(right.m_type != NumberKind) return false;
      double dleft = numberValue;
      double dright = right.numberValue;
      double diff = fabs(dleft - dright);
      if(std::isnan(diff) ||
	 (diff > std::numeric_limits<double>::epsilon())) return false;
    }
    break;
  case ComplexNumberKind:
    {
      if (m_type != right.ComplexNumberKind) return false;
      std::complex<double> dCleft = complexNumberValue;
      std::complex<double> dCright = right.complexNumberValue;
      double diff = abs(dCleft - dCright);
      if (std::isnan(diff) ||
        (diff > std::numeric_limits<double>::epsilon())) return false;
    }
    break;
  case SymbolKind:
    {
      if(right.m_type != SymbolKind) return false;

      return stringValue == right.stringValue;
    }
    break;
  case ListKind: 
    {
      if(right.m_type != ListKind) return false;

      return stringValue == right.stringValue;
    }
  break;  
  case LambdaKind: 
    {
      if(right.m_type != LambdaKind) return false;

      return stringValue == right.stringValue;
    }
  break;
  default:
    return false;
  }

  return true;
}

bool operator!=(const Atom & left, const Atom & right) noexcept{
  
  return !(left == right);
}


std::ostream & operator<<(std::ostream & out, const Atom & a){

  if(a.isNumber()){
    out << a.asNumber();
  }
  if(a.isComplexNumber()){
    out << a.asComplexNumber().real() << "," << a.asComplexNumber().imag();
  }
  if(a.isSymbol()){
    out << a.asSymbol();
  }
  return out;
}
