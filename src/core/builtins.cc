#include "rock/core/all.hh"

#include <functional>
#include <iostream>
#include <map>
#include <vector>

namespace rock {

Scope *builtins;

namespace {

Reference *brp;  // pointer to builtins reference

Init init(50, __FILE__, []() {
  builtins = new Scope();
  brp = new Reference(builtins);  // live until explicitly deleted
}, []() {
  delete brp;
});

void declareFunc(
    Scope *scope, const std::string &name,
    Reference(*f)(const Args&)) {
  scope->declare(name, Function::from(name, f));
}

// NOTE: Not all builtins values are set here.
// Builtin native classes are set in the cc file that implements the class.
Init init2(120, __FILE__, []() {
  declareFunc(builtins, "print", [](const Args& args) {
    checkargs(1, args);
    std::cout << args[0]->str() << std::endl;
    return args[0];
  });
  declareFunc(builtins, "L", [](const Args& args) -> Reference {
    return new List(args);
  });
  declareFunc(builtins, "assert", [](const Args& args) -> Reference {
    checkargs(1, args);
    if (!args[0]->truthy()) {
      throw exception("Assertion failed");
    }
    return nil;
  });
});

Init init3(2000, __FILE__, []() {
  declareFunc(builtins, "__declare", [](const Args &args) -> Reference {
    checkargs(2, args);
    checktype(classString, args[0]);
    std::string name = args[0].as<String>()->value;
    builtins->declare(name, args[1]);
    return nil;
  });
  Reference module = require("core/__prelude.rock");
  builtins->erase("__declare");
});

}  // namespace

}
