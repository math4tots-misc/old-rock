#include <iostream>
#include <cassert>
#include "rock/all.hh"
using namespace std;
using namespace rock;

int main() {
  assert(nil->debugstr() == "nil");
  assert(classNil->debugstr() == "Class(Nil)");
  assert(classNumber->debugstr() == "Class(Number)");
  assert(classClass->debugstr() == "Class(Class)");
  assert(Number::from(5.5)->debugstr() == "Number(5.500000)");

  {
    Reference<Class> r(new Class("foo/bar.rock:Sample"));
    assert(r->debugstr() == "Class(foo/bar.rock:Sample)");
  }

  cout << "tests pass!" << endl;
}
