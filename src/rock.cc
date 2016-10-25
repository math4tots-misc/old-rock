#include "rock.hh"
namespace rock {

//** stack
std::vector<Token*> stack;

//** core

static Class *makeStringClass() {
  Class *c = new Class();
  c->method_table["__str"] = [](Reference owner, const Args& args) {
    return Result(NORMAL, owner);
  };
  return c;
}

static Class *makeNativeFunctionClass() {
  Class *c = new Class();
  c->method_table["__call"] = [](Reference owner, const Args& args) {
    return static_cast<NativeFunction*>(owner.pointer)->function(args);
  };
  return c;
}

Class *classClass = new Class();
Class *classException = new Class();
Class *classNil = new Class();
Class *classNumber = new Class();
Class *classString = makeStringClass();
Class *classNativeFunction = makeNativeFunctionClass();
Reference classClassReference(classClass);
Reference classExceptionReference(classException);
Reference classNilReference(classNil);
Reference classNumberReference(classNumber);
Reference classStringReference(classString);
Reference classNativeFunctionReference(classNativeFunction);

Nil *nil = new Nil();
Reference nilReference(nil);

void acquire(Value *v) {
  v->reference_count++;
}

void release(Value *v) {
  v->reference_count--;
  if (v->reference_count <= 0) {
    delete v;
  }
}

Reference::Reference(): Reference(nil) {}

Result Reference::call(const std::string& name, const Args& args) const {
  Class *cls = pointer->getClass();
  auto pair = cls->method_table.find(name);
  if (pair == cls->method_table.end()) {
    return Result(
        EXCEPTION,
        Reference(new Exception("No such method: " + name)));
  }
  Method method = pair->second;
  return method(*this, args);
}

//** scope

Scope *makeGlobalScope() {
  Scope *scope = new Scope();
  scope->declare("Class", classClassReference);
  scope->declare("Exception", classExceptionReference);
  scope->declare("Nil", classNilReference);
  scope->declare("String", classStringReference);
  scope->declare("nil", nilReference);
  scope->declare("print", new NativeFunction([](const Args& args) {
    if (args.size() != 1) {
      return Result(EXCEPTION, new Exception("Expected exactly 1 arg"));
    }
    Result result = args[0].call("__str", {});

    switch (result.type) {
    case NORMAL:
      break;
    case EXCEPTION:
      return result;
    default:
      return Result(
          EXCEPTION,
          new Exception("Break or continue from native function"));
    }
    String *str = dynamic_cast<String*>(result.value.pointer);
    if (!str) {
      return Result(
          EXCEPTION, new Exception("__str didn't return a string!"));
    }
    std::cout << str->value << std::endl;
    return Result(NORMAL, nil);
  }));
  return scope;
}

}
