// TODO: Factor out tests

#include "rock/all.hh"
#include <iostream>
#include <memory>
#include <cassert>
using namespace std;
using namespace rock;

int main() {
  rock::initialize();

  {
    File f("<lex-test>", "a 'b' 3 4.4 +");
    vector<Token> tokens = lex(f);
    assert(tokens.size() == 6);
    assert(tokens[0].type == "ID");
    assert(tokens[0].value == "a");
    assert(tokens[1].type == "STR");
    assert(tokens[1].value == "b");
    assert(tokens[2].type == "NUM");
    assert(tokens[2].value == "3");
    assert(tokens[3].type == "NUM");
    assert(tokens[3].value == "4.4");
    assert(tokens[4].type == "+");
    assert(tokens[4].value == "");
    assert(tokens[5].type == "EOF");
    assert(tokens[5].value == "");
  }

  rock::finalize();
}
