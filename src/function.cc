#include "rock/function.hh"

#include "rock/class.hh"
#include "rock/result.hh"
#include "rock/exception.hh"

namespace rock {

namespace {

Class *makeClassFunction() {
  Class *c = new Class("Function");
  c->method_table["__call"] = [](Reference owner, const Args& args) {
    Function *f = dynamic_cast<Function*>(owner.pointer);
    if (f) {
      return f->function(args);
    }
    return Result(EXCEPTION, new Exception("this was not a function"));
  };
  return c;
}

}

Class *classFunction = makeClassFunction();
Reference clasFunctionReference(classFunction);

}
