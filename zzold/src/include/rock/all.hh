// printf "$(for f in `ls include/rock/**/*`; do printf '#include "'$f'"\n'; done)\n" | sed 's$include/$$'

// Dependencies:
//   1. base/*.hh
//   2. value/*.hh
//   3. ast/*.hh

#include "rock/ast/ast.hh"
#include "rock/ast/file.hh"
#include "rock/ast/lexer.hh"
#include "rock/ast/parser.hh"
#include "rock/base/debuggable.hh"
#include "rock/base/reference.hh"
#include "rock/base/referencecounted.hh"
#include "rock/base/result.hh"
#include "rock/value/class.hh"
#include "rock/value/exception.hh"
#include "rock/value/nil.hh"
#include "rock/value/number.hh"
#include "rock/value/scope.hh"
#include "rock/value/string.hh"
#include "rock/value/value.hh"
