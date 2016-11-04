// TODO: Factor out tests

#include "rock/all.hh"
#include <iostream>
#include <memory>
#include <cassert>
using namespace std;
using namespace rock;

int main() {
  rock::initialize();
  File file("<test>", "a b c");
  Token token(file, 0, "TEST");
  assert(
      token.locationMessage() ==
      "\nOn line 1 in file <test>"
      "\na b c"
      "\n*");
  rock::finalize();
}

