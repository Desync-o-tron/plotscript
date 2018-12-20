#include "catch.hpp"

#include "environment.hpp"
#include "semantic_error.hpp"

#include <cmath>

TEST_CASE( "Test default constructor", "[environment]" ) {

  Environment env;

  REQUIRE(env.is_known(Atom("pi")));
  REQUIRE(env.is_exp(Atom("pi")));
  REQUIRE(env.is_known(Atom("e")));
  REQUIRE(env.is_exp(Atom("e")));
  REQUIRE(env.is_known(Atom("I")));
  REQUIRE(env.is_exp(Atom("I")));

  REQUIRE(!env.is_known(Atom("hi")));
  REQUIRE(!env.is_exp(Atom("hi")));

  REQUIRE(env.is_proc(Atom("+")));
  REQUIRE(env.is_proc(Atom("-")));
  REQUIRE(env.is_proc(Atom("*")));
  REQUIRE(env.is_proc(Atom("/")));
  REQUIRE(env.is_proc(Atom("sin")));
  REQUIRE(env.is_proc(Atom("cos")));
  REQUIRE(env.is_proc(Atom("tan")));
  REQUIRE(env.is_proc(Atom("^")));
  REQUIRE(env.is_proc(Atom("sqrt")));
  REQUIRE(env.is_proc(Atom("ln")));
  REQUIRE(env.is_proc(Atom("real")));
  REQUIRE(env.is_proc(Atom("imag")));
  REQUIRE(env.is_proc(Atom("mag")));
  REQUIRE(env.is_proc(Atom("arg")));
  REQUIRE(env.is_proc(Atom("conj")));

  REQUIRE(!env.is_proc(Atom("op")));
}

TEST_CASE( "Test get expression", "[environment]" ) {
  Environment env;

  REQUIRE(env.get_exp(Atom("pi")) == Expression(std::atan2(0, -1)));
  REQUIRE(env.get_exp(Atom("hi")) == Expression());
}

TEST_CASE( "Test add expression", "[environment]" ) {
  Environment env;

  Expression a(Atom(1.0));
  env.add_exp(Atom("one"), a);
  REQUIRE(env.is_known(Atom("one")));
  REQUIRE(env.is_exp(Atom("one")));
  REQUIRE(env.get_exp(Atom("one")) == a);

  Expression b(Atom("hello"));
  env.add_exp(Atom("hi"), b);
  REQUIRE(env.is_known(Atom("hi")));
  REQUIRE(env.is_exp(Atom("hi")));
  REQUIRE(env.get_exp(Atom("hi")) == b);

  REQUIRE_THROWS_AS(env.add_exp(Atom(1.0), b), SemanticError);
}

TEST_CASE( "Test get built-in procedure", "[environment]" ) {
  Environment env;
  
  {
    INFO("default procedure")
    Procedure p1 = env.get_proc(Atom("doesnotexist"));
    Procedure p2 = env.get_proc(Atom("alsodoesnotexist"));
    REQUIRE(p1 == p2);
    std::vector<Expression> args;
    REQUIRE(p1(args) == Expression());
    REQUIRE(p2(args) == Expression());
  }

  {
    INFO("trying add procedure")

    std::vector<Expression> args;
    Procedure padd = env.get_proc(Atom("+"));
    args.emplace_back(1.0);
    args.emplace_back(2.0);
    REQUIRE(padd(args) == Expression(3.0));
    
    args.clear();
    args.emplace_back(std::complex<double>(0., 1.0));
    args.emplace_back(std::complex<double>(0., 2.0));
    REQUIRE(padd(args) == Expression(std::complex<double>(0., 3.0)));    

    args.clear();
    args.emplace_back(std::complex<double>(0., 1.0));
    args.emplace_back(Atom("asdf"));
    REQUIRE_THROWS_AS(padd(args), SemanticError);
  }

  {
    INFO("trying mult procedure")

    std::vector<Expression> args;
    Procedure pmult = env.get_proc(Atom("*"));
    args.emplace_back(std::complex<double>(0., 1.0));
    args.emplace_back(2.0);
    REQUIRE(pmult(args) == Expression(std::complex<double>(0., 2.0)));
    args.emplace_back(Atom("asdf"));
    REQUIRE_THROWS_AS(pmult(args), SemanticError);
  }

  {
    INFO("trying power procedure")

    std::vector<Expression> args;
    Procedure ppow = env.get_proc(Atom("^"));
    args.emplace_back(std::complex<double>(0., 1.0));
    args.emplace_back(2.0);
    REQUIRE(ppow(args) == Expression(std::complex<double>(-1.0, 0.0)));

    args.clear();
    args.emplace_back(1.0);
    args.emplace_back(2.0);
    REQUIRE(ppow(args) == Expression(1));

    args.clear();
    args.emplace_back(2.0);
    args.emplace_back(std::complex<double>(0., 1.0));
    REQUIRE(ppow(args) == Expression(std::complex<double>(0.769238901363972126578329993661270, 0.638961276313634801150032911464701)));

    args.clear();
    args.emplace_back(std::complex<double>(0., 1.0));
    args.emplace_back(std::complex<double>(0., 1.0));
    REQUIRE(ppow(args) == Expression(std::complex<double>(0.2078795763507619085469556198349787,0)));

    args.emplace_back(std::complex<double>(0., 1.0));
    REQUIRE_THROWS_AS(ppow(args), SemanticError);

    args.emplace_back(Atom("asdf"));
    REQUIRE_THROWS_AS(ppow(args), SemanticError);

  }

  {
    INFO("trying subneg procedure")
    
    std::vector<Expression> args;
    Procedure pneg = env.get_proc(Atom("-"));

    args.emplace_back(1.0);
    REQUIRE(pneg(args) == Expression(-1.));
    
    args.clear();
    args.emplace_back(std::complex<double>(0.0, 1.0));
    REQUIRE(pneg(args) == Expression(std::complex<double>(0.0, -1.0)));

    args.clear();
    args.emplace_back(1.0);
    args.emplace_back(2.0);
    REQUIRE(pneg(args) == Expression(-1.));

    args.clear();
    args.emplace_back(std::complex<double>(0.0, 1.0));
    args.emplace_back(2.0);
    REQUIRE(pneg(args) == Expression(std::complex<double>(-2.0, 1.0)));

    args.clear();
    args.emplace_back(2.0);
    args.emplace_back(std::complex<double>(0.0, 1.0));
    REQUIRE(pneg(args) == Expression(std::complex<double>(2.0, -1.0)));

    args.clear();
    args.emplace_back(std::complex<double>(0.0, 2.0));
    args.emplace_back(std::complex<double>(0.0, 1.0));
    REQUIRE(pneg(args) == Expression(std::complex<double>(0.0, 1.0)));
    
    args.emplace_back(std::complex<double>(0.0, 1.0));
    REQUIRE_THROWS_AS(pneg(args), SemanticError);

    args.emplace_back(Atom("asdf"));
    REQUIRE_THROWS_AS(pneg(args), SemanticError);

  }

  {
    INFO("trying sqrt procedure")

    std::vector<Expression> args;
    Procedure psqrt = env.get_proc(Atom("sqrt"));

    args.emplace_back(1.0);
    REQUIRE(psqrt(args) == Expression(1.0));

    args.clear();
    args.emplace_back(std::complex<double>(0.0, 1.0));
    REQUIRE(psqrt(args) == Expression(std::complex<double>(0.70710678118654752440, 0.70710678118654752440)));

    args.clear();
    args.emplace_back(-1.0);
    REQUIRE(psqrt(args) == Expression(std::complex<double>(0.0, 1.0)));

    args.clear();
    args.emplace_back(-1.0);
    args.emplace_back(-1.0);
    REQUIRE_THROWS_AS(psqrt(args), SemanticError);

    args.emplace_back(Atom("asdf"));
    REQUIRE_THROWS_AS(psqrt(args), SemanticError);
  }

  {
    INFO("trying div procedure")

    std::vector<Expression> args;
    Procedure pdiv = env.get_proc(Atom("/"));

    args.emplace_back(1.0);
    args.emplace_back(2.0);
    REQUIRE(pdiv(args) == Expression(.5));

    args.clear();
    args.emplace_back(1.0);
    args.emplace_back(std::complex<double>(0.0, 1.0));
    REQUIRE(pdiv(args) == Expression(std::complex<double>(0., -1.)));

    args.clear();
    args.emplace_back(std::complex<double>(0.0, 1.0));
    args.emplace_back(1.0);
    REQUIRE(pdiv(args) == Expression(std::complex<double>(0., 1.)));

    args.clear();
    args.emplace_back(std::complex<double>(0.0, 2.0));
    args.emplace_back(std::complex<double>(2.0, 1.0));
    REQUIRE(pdiv(args) == Expression(std::complex<double>(0.4, 0.8)));

    args.emplace_back(std::complex<double>(0.0, 1.0));
    REQUIRE_THROWS_AS(pdiv(args), SemanticError);

    args.clear();
    args.emplace_back(2);
    REQUIRE(pdiv(args) == Expression(1./2));

    args.clear();
    args.emplace_back(2);
    REQUIRE(pdiv(args) == Expression(1. / 2));

    args.clear();
    args.emplace_back(Atom(std::complex<double>(0.0, 1.0)));
    REQUIRE(pdiv(args) == Expression(Atom(std::complex<double>(0.0, -1.0))));

  }

  {
    INFO("trying ln procedure")

    std::vector<Expression> args;
    Procedure pln = env.get_proc(Atom("ln"));

    args.emplace_back(2.7182818284590452353602);
    REQUIRE(pln(args) == Expression(1.));
    
    args.clear();
    args.emplace_back(1.0);
    REQUIRE(pln(args) == Expression(0.0));

    args.emplace_back(std::complex<double>(0.0, 1.0));
    REQUIRE_THROWS_AS(pln(args), SemanticError);

    args.clear();
    args.emplace_back(-1.0);
    REQUIRE_THROWS_AS(pln(args), SemanticError);

    args.clear();
    args.emplace_back(Atom("asdf"));
    REQUIRE_THROWS_AS(pln(args), SemanticError);
  }

  {
    INFO("trying sin procedure")

    std::vector<Expression> args;
    Procedure psin = env.get_proc(Atom("sin"));
    args.emplace_back(0.);
    REQUIRE(psin(args) == Expression(0.));

    args.emplace_back(std::complex<double>(0.0, 1.0));
    REQUIRE_THROWS_AS(psin(args), SemanticError);

    args.clear();
    args.emplace_back(Atom("asdf"));
    REQUIRE_THROWS_AS(psin(args), SemanticError);
  }

  {
    INFO("trying cos procedure")

    std::vector<Expression> args;
    Procedure pcos = env.get_proc(Atom("cos"));
    args.emplace_back(0.);
    REQUIRE(pcos(args) == Expression(1));

    args.emplace_back(std::complex<double>(0.0, 1.0));
    REQUIRE_THROWS_AS(pcos(args), SemanticError);

    args.emplace_back(Atom("asdf"));
    REQUIRE_THROWS_AS(pcos(args), SemanticError);
  }

  {
    INFO("trying tan procedure")

    std::vector<Expression> args;
    Procedure ptan = env.get_proc(Atom("tan"));
    args.emplace_back(0.);
    REQUIRE(ptan(args) == Expression(0));

    args.emplace_back(std::complex<double>(0.0, 1.0));
    REQUIRE_THROWS_AS(ptan(args), SemanticError);

    args.emplace_back(Atom("asdf"));
    REQUIRE_THROWS_AS(ptan(args), SemanticError);
  }

  {
    INFO("trying real procedure")

    std::vector<Expression> args;
    Procedure preal = env.get_proc(Atom("real"));

    args.emplace_back(std::complex<double>(2.0, 1.0));
    REQUIRE(preal(args) == Expression(2));

    args.emplace_back(std::complex<double>(0.0, 1.0));
    REQUIRE_THROWS_AS(preal(args), SemanticError);

    args.emplace_back(Atom("asdf"));
    REQUIRE_THROWS_AS(preal(args), SemanticError);
  }

  {
    INFO("trying imag procedure")

    std::vector<Expression> args;
    Procedure pimag = env.get_proc(Atom("imag"));

    args.emplace_back(std::complex<double>(2.0, 1.0));
    REQUIRE(pimag(args) == Expression(1.));

    args.emplace_back(std::complex<double>(0.0, 1.0));
    REQUIRE_THROWS_AS(pimag(args), SemanticError);

    args.emplace_back(Atom("asdf"));
    REQUIRE_THROWS_AS(pimag(args), SemanticError);
  }

  {
    INFO("trying mag procedure")

    std::vector<Expression> args;
    Procedure pmag = env.get_proc(Atom("mag"));

    args.emplace_back(std::complex<double>(2.0, 1.0));
    REQUIRE(pmag(args) == Expression(2.236067977499789696409));

    args.clear();
    args.emplace_back(std::complex<double>(0., 1.0));
    REQUIRE(pmag(args) == Expression(1));

    args.emplace_back(std::complex<double>(0.0, 1.0));
    REQUIRE_THROWS_AS(pmag(args), SemanticError);

    args.emplace_back(Atom("asdf"));
    REQUIRE_THROWS_AS(pmag(args), SemanticError);
  }

  {
    INFO("trying arg procedure")

    std::vector<Expression> args;
    Procedure parg = env.get_proc(Atom("arg"));

    args.emplace_back(std::complex<double>(4.0, 1.0));
    REQUIRE(parg(args) == Expression(0.24497866312686415));

    args.emplace_back(std::complex<double>(0.0, 1.0));
    REQUIRE_THROWS_AS(parg(args), SemanticError);

    args.emplace_back(Atom("asdf"));
    REQUIRE_THROWS_AS(parg(args), SemanticError);
  }

  {
    INFO("trying conj procedure")

    std::vector<Expression> args;
    Procedure pconj = env.get_proc(Atom("conj"));

    args.emplace_back(std::complex<double>(4.0, 1.0));
    REQUIRE(pconj(args) == Expression(std::complex<double>(4.0, -1.0)));

    args.emplace_back(std::complex<double>(0.0, 1.0));
    REQUIRE_THROWS_AS(pconj(args), SemanticError);

    args.emplace_back(Atom("asdf"));
    REQUIRE_THROWS_AS(pconj(args), SemanticError);
  }

}


TEST_CASE( "Test reset", "[environment]" ) {
  Environment env;

  Expression a(Atom(1.0));
  env.add_exp(Atom("one"), a);
  Expression b(Atom("hello"));
  env.add_exp(Atom("hi"), b);

  env.reset();
  REQUIRE(!env.is_known(Atom("one")));
  REQUIRE(!env.is_exp(Atom("one")));
  REQUIRE(env.get_exp(Atom("one")) == Expression());
  REQUIRE(!env.is_known(Atom("hi")));
  REQUIRE(!env.is_exp(Atom("hi")));
  REQUIRE(env.get_exp(Atom("hi")) == Expression());
}

TEST_CASE("Test semantic errors", "[environment]") {
  
  Environment env;

}


TEST_CASE( "Testing lists", "[environment]" ) {

  Environment env;

  {
    INFO("trying first procedure")

    Procedure plist = env.get_proc(Atom("list"));

    std::vector<Expression> args;
    args.emplace_back(std::complex<double>(4.0, 1.0));
    args.emplace_back(std::complex<double>(5.0, 1.0));
    args.emplace_back(std::complex<double>(6.0, 1.0));
    
    Expression list(Atom("list"));
    list = plist(args);

    Procedure pfirst = env.get_proc(Atom("first"));
    std::vector<Expression> vec = { list };
    REQUIRE(pfirst(vec) == Expression(std::complex<double>(4.0, 1.0)));

    vec.push_back(list);
    REQUIRE_THROWS_AS(pfirst(vec), SemanticError);    
  }

  {
    INFO("trying rest procedure")

    Procedure plist = env.get_proc(Atom("list"));

    std::vector<Expression> args;
    args.emplace_back(std::complex<double>(4.0, 1.0));
    args.emplace_back(std::complex<double>(5.0, 1.0));
    args.emplace_back(std::complex<double>(6.0, 1.0));

    Expression list(Atom("list"));
    list = plist(args);
    
    Expression list2(Atom("list"));
    std::vector<Expression> args2;
    args2.emplace_back(std::complex<double>(5.0, 1.0));
    args2.emplace_back(std::complex<double>(6.0, 1.0));
    list2 = plist(args2);

    Procedure pRest = env.get_proc(Atom("rest"));
    std::vector<Expression> vec = { list };
    Expression rest = pRest(vec);
    REQUIRE( rest == plist(args2));
    
    vec.push_back(list);
    REQUIRE_THROWS_AS(pRest(vec), SemanticError);

    //vec = {};
    //REQUIRE_THROWS_AS(pRest(vec), SemanticError);
    
  }
  {
    INFO("trying length procedure")

    Procedure plist = env.get_proc(Atom("list"));
    Procedure plength = env.get_proc(Atom("length"));

    std::vector<Expression> args;
    args.emplace_back(std::complex<double>(4.0, 1.0));
    args.emplace_back(std::complex<double>(5.0, 1.0));
    args.emplace_back(std::complex<double>(6.0, 1.0));

    Expression list(Atom("list"));
    list = plist(args);
    std::vector<Expression> listVec = { list };
    
    Expression len = plength(listVec);
    REQUIRE(len == Expression(Atom(3)));

    listVec.push_back(list);

    REQUIRE_THROWS_AS(plength(listVec), SemanticError);
    
    listVec[0] = Expression(Atom(1));
    REQUIRE_THROWS_AS(plength(listVec), SemanticError);

  }  
  
  {
    INFO("trying append procedure")

    Procedure plist = env.get_proc(Atom("list"));
    Procedure pAppend = env.get_proc(Atom("append"));

    std::vector<Expression> args;
    args.emplace_back(std::complex<double>(4.0, 1.0));
    args.emplace_back(std::complex<double>(5.0, 1.0));

    Expression list(Atom("list"));
    list = plist(args);

    std::vector<Expression> inputVector;
    inputVector.push_back(list);
    inputVector.emplace_back(std::complex<double>(6.0, 1.0));

    Expression fullList = pAppend(inputVector);
    
    std::vector<Expression> listVec;
    listVec.emplace_back(std::complex<double>(4.0, 1.0));
    listVec.emplace_back(std::complex<double>(5.0, 1.0));
    listVec.emplace_back(std::complex<double>(6.0, 1.0));
    list = plist(listVec);
    
    REQUIRE(fullList == list);

    inputVector[0] = Expression(Atom(1));
    REQUIRE_THROWS_AS(pAppend(inputVector), SemanticError);

    inputVector.push_back(list);
    REQUIRE_THROWS_AS(pAppend(inputVector), SemanticError);
  }
  {
    INFO("trying join procedure")
    Procedure pList = env.get_proc(Atom("list"));
    Procedure pJoin = env.get_proc(Atom("join"));

    std::vector<Expression> list1input;
    std::vector<Expression> list2input;
    std::vector<Expression> list3input;

    list1input.emplace_back(std::complex<double>(4.0, 1.0));
    list2input.emplace_back(std::complex<double>(4.0, 1.0));
    list3input.emplace_back(std::complex<double>(4.0, 1.0));
    list3input.emplace_back(std::complex<double>(4.0, 1.0));

    Expression list1 = pList(list1input);
    Expression list2 = pList(list2input);
    Expression list3 = pList(list3input);

    std::vector<Expression> joinInput = { list1, list2 };
    Expression list12 = pJoin(joinInput);
    REQUIRE(list12 == list3);
    
    joinInput.pop_back();
    REQUIRE_THROWS_AS(pJoin(joinInput), SemanticError);
  }
  
  {
    INFO("trying range procedure")
    Procedure pList = env.get_proc(Atom("list"));
    Procedure pRange = env.get_proc(Atom("range"));

    std::vector<Expression> rangeinput;
    std::vector<Expression> rangeoutput;
    
    rangeinput.emplace_back(0.0);
    rangeinput.emplace_back(2.0);
    rangeinput.emplace_back(1.0);
    rangeoutput.emplace_back(0.0);
    rangeoutput.emplace_back(1.0);
    rangeoutput.emplace_back(2.0);

    Expression rhs = pList(rangeoutput);
    Expression lhs = pRange(rangeinput);

    REQUIRE(lhs == rhs);
            
    rangeinput.pop_back();
    REQUIRE_THROWS_AS(pRange(rangeinput), SemanticError);

    rangeinput.emplace_back(-1.);
    REQUIRE_THROWS_AS(pRange(rangeinput), SemanticError);
  }
  
  //{
  //  INFO("trying apply procedure")
  //  Procedure pList = env.get_proc(Atom("list"));
  //  Procedure pApply = env.get_proc(Atom("apply"));

  //  std::vector<Expression> listIn;
  //  //std::vector<Expression> desiredOut;
  //  //std::vector<Expression> ActualOut;
  //  
  //  listIn.emplace_back(1.0);
  //  listIn.emplace_back(2.0);
  //  std::vector<Expression> inputVec;
  //  inputVec.emplace_back(Expression(Atom("-")));
  //  Expression list = pList(listIn);
  //  inputVec.emplace_back(list);


  //  Expression desiredOut(2.0);
  //  //desiredOut.emplace_back(4.0);
  //  //Expression desiredOutList = pList(desiredOut);
  //  Expression lhs = pApply(inputVec);
  //  REQUIRE(lhs == desiredOut);

  //  
  //  listIn.pop_back();
  //  REQUIRE_THROWS_AS(pApply(inputVec), SemanticError);

  //  listIn.emplace_back(2.0);
  //  //rangeinput.emplace_back(-1.);
  //  REQUIRE_THROWS_AS(pApply(inputVec), SemanticError);
  //}

}

TEST_CASE("Testing changing the map", "[environment]") {

  Environment env;

  {
    INFO("trying add exp procedure")

    Procedure pList = env.get_proc(Atom("list"));
    Expression I = env.get_exp(Atom("I"));
    env.add_exp(Atom("I"), Expression(2));
    REQUIRE(I != env.get_exp(Atom("I")));
  }

}
TEST_CASE("Testing copying the map", "[environment]") {

  Environment env;
  Environment env2(env);
  REQUIRE(env.get_exp(Atom("I")) == env2.get_exp(Atom("I")));
}

//TEST_CASE("Testing lambda function", "[environment]") {
//  using namespace std;
//  Environment env;
//  Expression lambda(Atom("lambda"));
//  vector<Expression> prelist;
//  prelist.push_back()
//  //lambda.appendExpression()
//
//}

