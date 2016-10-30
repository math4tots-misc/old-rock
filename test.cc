#include "rock/all.hh"
#include <iostream>
using namespace std;
using namespace rock;

int main() {
  rock::init();
  cout << classClass->debug() << endl;

  File f("a", "b");
  File g(f);
}
