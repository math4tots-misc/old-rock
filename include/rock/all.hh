// printf "$(for f in `ls include/rock/**/*`; do printf '#include "'$f'"\n'; done)\n"
// Then manually remove extraneous 'include'.
#include "rock/base/debuggable.hh"
#include "rock/base/reference.hh"
#include "rock/base/referencecounted.hh"
#include "rock/value/class.hh"
#include "rock/value/nil.hh"
#include "rock/value/number.hh"
#include "rock/value/value.hh"
