/*! \file atom.hpp
Defines the Atom type and associated functions.
 */
#ifndef ATOM_HPP
#define ATOM_HPP

#include "token.hpp"
#include <complex>

/*! \class Atom
\brief A variant type that may be a Number or Symbol or the default type None.

This class provides value semantics.
*/
class Atom {
public:

  /// Construct a default Atom of type None
  Atom();

  /// Construct an Atom of type Number with value
  Atom(double value);

  /// Construct an Atom of type Number with complex value
  Atom(std::complex<double> value);

  /// Construct an Atom of type Symbol named value
  Atom(const std::string & value);

  /// Construct an Atom directly from a Token
  Atom(const Token & token);

  /// Copy-construct an Atom
  Atom(const Atom & x);

  /// Assign an Atom
  Atom & operator=(const Atom & x);

  /// Atom destructor
  ~Atom();

  /// predicate to determine if an Atom is of type None
  bool isNone() const noexcept;

  /// predicate to determine if an Atom is of type Number
  bool isNumber() const  noexcept;
  
  /// predicate to determine if an Atom is of type List
  bool isList() const  noexcept;

  /// predicate to determine if an Atom is of type List
  bool isLambda() const noexcept;

  /// predicate to determine if an Atom is of type ComplexNumber
  bool isComplexNumber() const  noexcept;

  /// predicate to determine if an Atom is of type Symbol
  bool isSymbol() const noexcept;

  /// value of Atom as a number, return 0 if not a Number
  double asNumber() const noexcept;

  /// value of Atom as a complex number, return (0.,0.) if not a complex Number
  std::complex<double> asComplexNumber() const noexcept;

  /// value of Atom as a symbol/list/lambda, returns empty-string if not a Symbol or a list or lambda
  /// returns "list" if ListKind and "lambda if LambdaKind
  std::string asSymbol() const noexcept;

  /// equality comparison based on type and value
  bool operator==(const Atom & right) const noexcept;

private:

  // internal enum of known types
  enum Type {NoneKind, NumberKind, ComplexNumberKind, SymbolKind, ListKind, LambdaKind};

  // track the type
  Type m_type;

  // values for the known types. Note the use of a union requires care
  // when setting non POD values (see setSymbol)
  union {
    double numberValue;
    std::complex<double> complexNumberValue; 
    std::string stringValue;
  };

  // helper to set type and value of Number
  void setNumber(double value);

  // helper to set type and value of complex Number
  void setComplexNumber(std::complex<double> value);

  // helper to set type and value of Symbol
  void setSymbol(const std::string & value);
  
  //
  void setList();
  
  //
  void setLambda();

};

/// inequality comparison for Atom
bool operator!=(const Atom &left, const Atom & right) noexcept;

/// output stream rendering
std::ostream & operator<<(std::ostream & out, const Atom & a);

#endif
