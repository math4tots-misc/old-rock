#include "rock/all.hh"
#include <iostream>
#include <memory>
#include <cassert>
using namespace std;
using namespace rock;

int main() {
  rock::init();

  {
    // lex test
    File f("<lex-test>", "a 'b' 3 4.4 +");
    vector<Token> tokens = lex(f);
    assert(tokens.size() == 6);
    assert(tokens[0].type == "ID");
    assert(tokens[0].value == "a");
  }

  cout << classClass->debug() << endl;
  cout << classException->debug() << endl;

  File f("a", "b");
  File g(f);

  std::unique_ptr<Unit> unit = parseFile("<test>", "a + b");
  cout << unit->node->debug() << endl;
  Scope scope;
  cout << unit->node->eval(scope).debug() << endl;
}
