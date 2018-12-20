#include "catch.hpp"
#include "semantic_error.hpp"
#include "environment.hpp"
#include "expression.hpp"

TEST_CASE( "Test default expression", "[expression]" ) {

  Expression exp;

  REQUIRE(!exp.isHeadNumber());
  REQUIRE(!exp.isHeadSymbol());
}

TEST_CASE( "Test double expression", "[expression]" ) {

  Expression exp(6.023);

  REQUIRE(exp.isHeadNumber());
  REQUIRE(!exp.isHeadSymbol());
}


TEST_CASE( "Test symbol expression", "[expression]" ) {

  Expression exp(Atom("asymbol"));

  REQUIRE(!exp.isHeadNumber());
  REQUIRE(exp.isHeadSymbol());
}

TEST_CASE("Test list expression", "[expression]") {

  Expression exp(Atom("list"));

  REQUIRE(!exp.isHeadNumber());
  REQUIRE(exp.isHeadList());
}

TEST_CASE("Test property type getters", "[expression]") {
  Expression exp(Atom(22));
  Expression point(Atom("\"point\""));
  exp.pList["\"object-name\""] = point;
  REQUIRE(exp.isTypePoint());
  REQUIRE(!exp.isTypeLine());
}

TEST_CASE("Test lproperty type getters 2", "[expression]") {
  Expression exp(Atom(22));
  Expression line(Atom("\"line\""));
  exp.pList["\"object-name\""] = line;
  REQUIRE(!exp.isTypePoint());
  REQUIRE(!exp.isTypeText());
  REQUIRE(exp.isTypeLine());
}

TEST_CASE("Test lproperty type getters 3", "[expression]") {
  Expression exp(Atom(22));
  Expression text(Atom("\"text\""));
  exp.pList["\"object-name\""] = text;
  REQUIRE(!exp.isTypePoint());
  REQUIRE(exp.isTypeText());
  REQUIRE(!exp.isTypeLine());
}

//TEST_CASE("make a bad lambda", "[expression]")
//{
//  Environment env;
//  Expression e;
//  Expression e2;
//  std::vector<Expression> vec;
//  vec.push_back(e);
//  vec.push_back(e2);
//  REQUIRE_THROWS_AS(callALambda(Atom("I"), vec, env), SemanticError);
//
//
//}


