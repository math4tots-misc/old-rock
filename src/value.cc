#include "rock/all.hh"


namespace rock {

bool Value::operator==(Value v) const {
  if (type != v.type) {
    return false;
  }
  switch (type) {
    case Type::BOOL: bewl == v.bewl;
    case Type::NUMBER: num == v.num;
    case Type::OBJECT: call(xObject, "equals", )
  }
  return
      klass == v.klass &&
      (klass ==
}

}  // namespace rock