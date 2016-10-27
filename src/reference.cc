#include "rock/all.hh"

namespace rock {

template <>
Reference<Value>::Reference(): pointer(nil.pointer) {}

}
