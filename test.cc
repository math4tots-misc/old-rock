#include "rock/all.hh"
#include <iostream>
#include <memory>
using namespace std;
using namespace rock;

int main() {
  rock::init();
  cout << classClass->debug() << endl;
  cout << classException->debug() << endl;

  File f("a", "b");
  File g(f);

  std::unique_ptr<Unit> unit = parseFile("<test>", "a + b");
}
