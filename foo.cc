#include <iostream>
#include <cassert>
#include "rock/all.hh"
using namespace std;
using namespace rock;

int main() {
  // Scope *globals = makeGlobals();
  cout << "AAA" << eval("print('hi')").debugstr() << "BBBB" << endl;
  cout << "foo" << endl;
  // eval(globals, "print('hi')");
}
