#include "rock.hh"
namespace rock {

//** stack
std::vector<Token*> stack;

//** core
Class *classClass = new Class();
Class *classException = new Class();
Class *classNil = new Class();
Class *classNumber = new Class();
Class *classString = new Class();
Reference classClassReference(classClass);
Reference classExceptionReference(classException);
Reference classNilReference(classNil);
Reference classNumberReference(classNumber);
Reference classStringReference(classString);

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
  Method *method = pair->second;
  return (*method)(*this, args);
}

}
