#include "catch.hpp"

#include "atom.hpp"

TEST_CASE( "Test constructors", "[atom]" ) {

  {
    INFO("Default Constructor");
    Atom a;

    REQUIRE(a.isNone());
    REQUIRE(!a.isNumber());
    REQUIRE(!a.isSymbol());
  }

  {
    INFO("Number Constructor");
    Atom a(1.0);

    REQUIRE(!a.isNone());
    REQUIRE(a.isNumber());
    REQUIRE(!a.isSymbol());
  }

  {
    INFO("Complex Number Constructor");
    Atom a( std::complex<double>(0,1) );

    REQUIRE(!a.isNone());
    REQUIRE(a.isComplexNumber()); 
    REQUIRE(!a.isSymbol());
  }

  {
    INFO("Symbol Constructor");
    Atom a("hi");

    REQUIRE(!a.isNone());
    REQUIRE(!a.isNumber());
    REQUIRE(a.isSymbol());
  }

  {
    INFO("List Constructor");
    Atom a("list");

    REQUIRE(!a.isNone());
    REQUIRE(!a.isNumber());
    REQUIRE(a.isList());
  }

  {
    INFO("Lambda Constructor");
    Atom a("lambda");

    REQUIRE(!a.isNone());
    REQUIRE(!a.isNumber());
    REQUIRE(a.isLambda());
  }

  {
    INFO("Token Constructor");
    Token t("hi");
    Atom a(t);

    REQUIRE(!a.isNone());
    REQUIRE(!a.isNumber());
    REQUIRE(a.isSymbol());

    //Token t1("list");
    //Atom a1(t1);

    //REQUIRE(!a1.isNone());
    //REQUIRE(!a1.isNumber());
    //REQUIRE(a1.isList());    
    //
    //Token t2("lambda");
    //Atom a2(t2);

    //REQUIRE(!a2.isNone());
    //REQUIRE(!a2.isNumber());
    //REQUIRE(a2.isLambda());
  }

  {
    INFO("Copy Constructor");
    Atom a("hi");
    Atom b(1.0);
    
    Atom c = a;
    REQUIRE(!a.isNone());
    REQUIRE(!c.isNumber());
    REQUIRE(c.isSymbol());

    Atom d = b;
    REQUIRE(!a.isNone());
    REQUIRE(d.isNumber());
    REQUIRE(!d.isSymbol());

    Atom e(std::complex<double>(0, 1));
    Atom f(e);
    REQUIRE(f == e);

    Atom g("list");
    Atom h(g);
    REQUIRE(g == h);

    Atom i("lambda");
    Atom j(i);
    REQUIRE(i == j);
    
  }
}

TEST_CASE( "Test assignment", "[atom]" ) {

  {
    INFO("default to default");
    Atom a;
    Atom b;
    b = a;
    REQUIRE(b.isNone());
    REQUIRE(!b.isNumber());
    REQUIRE(!b.isSymbol());
  }

  {
    INFO("default to number");
    Atom a;
    Atom b(1.0);
    b = a;
    REQUIRE(b.isNone());
    REQUIRE(!b.isNumber());
    REQUIRE(!b.isSymbol());
  }

  {
    INFO("default to complex number");
    Atom a;
    Atom b(std::complex<double>(0.,1.0));
    b = a;
    REQUIRE(b.isNone());
    REQUIRE(!b.isNumber());
    REQUIRE(!b.isSymbol());
  }

  {
    INFO("default to symbol");
    Atom a;
    Atom b("hi");
    b = a;
    REQUIRE(b.isNone());
    REQUIRE(!b.isNumber());
    REQUIRE(!b.isSymbol());
  }

  {
    INFO("number to default");
    Atom a(1.0);
    Atom b;
    b = a;
    REQUIRE(b.isNumber());
    REQUIRE(b.asNumber() == 1.0);
  }

  {
    INFO("complex number to default");
    Atom a(std::complex<double>(0., 1.0));
    Atom b;
    b = a;
    REQUIRE(b.isComplexNumber());
    REQUIRE(b.asComplexNumber() == std::complex<double>(0., 1.0));
  }

  {
    INFO("number to number");
    Atom a(1.0);
    Atom b(2.0);
    b = a;
    REQUIRE(b.isNumber());
    REQUIRE(b.asNumber() == 1.0);
  }

  {
    INFO("complex number to complex number");
    Atom a(std::complex<double>(0., 1.0));
    Atom b(std::complex<double>(0., 2.0));
    b = a;
    REQUIRE(b.isComplexNumber());
    REQUIRE(b.asComplexNumber() == std::complex<double>(0., 1.0));
  }

  {
    INFO("number to symbol");
    Atom a("hi");
    Atom b(1.0);
    b = a;
    REQUIRE(b.isSymbol());
    REQUIRE(b.asSymbol() == "hi");
  }  
  
  {
    INFO("complex number to symbol");
    Atom a("hi");
    Atom b(std::complex<double>(0., 1.0));
    b = a;
    REQUIRE(b.isSymbol());
    REQUIRE(b.asSymbol() == "hi");
  }

  {
    INFO("symbol to default");
    Atom a("hi");
    Atom b;
    b = a;
    REQUIRE(b.isSymbol());
    REQUIRE(b.asSymbol() == "hi");
  }

  {
    INFO("symbol to number");
    Atom a("hi");
    Atom b(1.0);
    b = a;
    REQUIRE(b.isSymbol());
    REQUIRE(b.asSymbol() == "hi");
  }

  {
    INFO("symbol to complex number");
    Atom a("hi");
    Atom b(std::complex<double>(0., 1.0));
    b = a;
    REQUIRE(b.isSymbol());
    REQUIRE(b.asSymbol() == "hi");
  }

  {
    INFO("symbol to symbol");
    Atom a("hi");
    Atom b("bye");
    b = a;
    REQUIRE(b.isSymbol());
    REQUIRE(b.asSymbol() == "hi");
  }

  {
    INFO("symbol to lambda");
    Atom a("lambda");
    Atom b("bye");
    b = a;
    REQUIRE(b.isLambda());
    REQUIRE(b.asSymbol() == "lambda");
  }

}

TEST_CASE( "test comparison", "[atom]" ) {

  {
    INFO("compare default to default");
    Atom a;
    Atom b;
    REQUIRE(a == b);
  }

  {
    INFO("compare default to number");
    Atom a;
    Atom b(1.0);
    REQUIRE(a != b);
  }

  {
    INFO("compare default to complex number");
    Atom a;
    Atom b(std::complex<double>(0., 1.0));
    REQUIRE(a != b);
  }

  {
    INFO("compare default to symbol");
    Atom a;
    Atom b("hi");
    REQUIRE(a != b);
  }

  {
    INFO("compare number to default");
    Atom a(1.0);
    Atom b;
    REQUIRE(a != b);
  }

  {
    INFO("compare complex number to default");
    Atom a(std::complex<double>(0., 1.0));
    Atom b;
    REQUIRE(a != b);
  }

  {
    INFO("compare number to number");
    Atom a(1.0);
    Atom b(1.0);
    Atom c(2.0);
    REQUIRE(a == b);
    REQUIRE(a != c);
  }

  {
    INFO("compare complex number to number");
    Atom a(std::complex<double>(0., 1.0));
    Atom b(std::complex<double>(0., 1.0));
    Atom c(2.0);
    REQUIRE(a == b);
    REQUIRE(a != c);
  }

  {
    INFO("compare complex number to complex number");
    Atom a(std::complex<double>(0., 1.0));
    Atom b(std::complex<double>(0., 1.0));
    Atom c(std::complex<double>(0., 2.0));
    REQUIRE(a == b);
    REQUIRE(a != c);
  }

  {
    INFO("compare number to symbol");
    Atom a(1.0);
    Atom b("hi");
    REQUIRE(a != b);
  }

  {
    INFO("compare symbol to default");
    Atom a("hi");
    Atom b;
    REQUIRE(a != b);
  }

  {
    INFO("compare symbol to number");
    Atom a("hi");
    Atom b(1.0);
    REQUIRE(a != b);
  }

  {
    INFO("compare symbol to complex number");
    Atom a("hi");
    Atom b(std::complex<double>(0., 1.0));
    REQUIRE(a != b);
  }

  {
    INFO("compare symbol to symbol");
    Atom a("hi");
    Atom b("hi");
    Atom c("bye");
    REQUIRE(a == b);
    REQUIRE(a != c);
  }

  {
    INFO("compare symbol to lambda");
    Atom a("hi");
    Atom b("hi");
    Atom c("lambda");
    REQUIRE(a == b);
    REQUIRE(a != c);
  }

}