#include <iostream>
#include <cassert>
#include "rock/function.hh"
#include "rock/globals.hh"
#include "rock/nil.hh"
#include "rock/scope.hh"

namespace rock {

Scope *makeGlobals() {
  Scope *scope = new Scope();
  assert(scope->declare("print", new Function([](const Args& args) {
    if (args.size() != 1) {
      return Result(EXCEPTION, new Exception("Expected 1 arg"));
    }
    std::cout << args[0].debugstr() << std::endl;
    return Result(NORMAL, nil);
  })).type == NORMAL);
  return scope;
}

}
