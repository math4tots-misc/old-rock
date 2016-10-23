// g++ test.cc --std=c++11 -Iinclude -Wall -Werror -Wpedantic && ./a.out
#include <iostream>
#include "rock/all.hh"
using namespace rock;
using namespace std;

int main() {
  P s = String::from("Hello world!");
  Token t(nullptr, 0);
  Ast *b = new Block(t, std::vector<Ast*>({}));
  Scope scope;
  Result result = b->eval(scope);
  cout << (result.type == NORMAL) << endl;
  cout << (result.value.ptr == &nil) << endl;
}
