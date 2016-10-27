#include "rock/nil.hh"

#include "rock/class.hh"

namespace rock {

Class *classNil = new Class("Nil");
Reference clasNilReference(classNil);
Nil *nil = new Nil();

}
