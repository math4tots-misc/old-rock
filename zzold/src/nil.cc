#include "rock/all.hh"

namespace rock {

Reference<Class> classNil(new Class("Nil", {classValue}));
Reference<Nil> nil(new Nil);

}
