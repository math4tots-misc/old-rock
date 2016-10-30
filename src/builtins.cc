#include "rock/all.hh"

#include <functional>
#include <iostream>
#include <map>
#include <vector>

namespace rock {

namespace {

Reference *brp;  // pointer to builtins reference

Init init(5, __FILE__, []() {
  builtins = new Scope();
  brp = new Reference(builtins);  // live until explicitly deleted
}, []() {
  delete brp;
});
}

Scope *builtins;

// NOTE: Not all builtins values are set here.
// Builtin native classes are set in the cc file that implements the class.
Init init(10, __FILE__, []() {
  builtins->declare("print", new Function("print", [](const Args& args) {
    if (args.size() != 1) {
      return Result(
          Result::Type::EXCEPTION,
          new Exception("Expected 1 argument"));
    }
    std::cout << args[0]->str() << std::endl;
    return Result(Result::Type::OK, args[0]);
  }));
});

}
