// First ever set of tests.
// Just sanity checks as I write more code.
// g++ test/start.cc --std=c++11 -Iinclude -Wall -Werror -Wpedantic && ./a.out
#include <assert.h>
#include <iostream>

#include "rock/all.hh"

using namespace std;
using namespace rock;

int main() {
  P s = String::from("Hello world!");
  Token t(nullptr, 0);
  {
    // Executing an empty block does nothing and returns nil.
    Scope scope;
    Ast *b = new Block(t, std::vector<Ast*>({}));
    Result result = b->eval(scope);
    assert(result.type == NORMAL);
    assert(result.value.ptr == &nil);
  }
  {
    // When referring to unknown variable, throws exception
    Scope scope;
    Result result = (new Name(t, "foo"))->eval(scope);
    assert(result.type == EXCEPTION);
    auto e = dynamic_cast<Exception*>(result.value.ptr);
    assert(e);
    assert(e->message == "No such variable: foo");
  }
  {
    // Globals
    Globals globals;
    Result result = (new Name(t, "String"))->eval(globals);
    assert(result.type == NORMAL);
    auto c = dynamic_cast<Class*>(result.value.ptr);
    assert(c);
    assert(c->name == "String");
  }
  cout << "Tests pass!" << endl;
}
