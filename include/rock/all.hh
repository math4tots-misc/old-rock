// ls include/rock/ | awk '{ print "#include \"rock/"$0"\"" }' | tail -n +2
#include "rock/ast.hh"
#include "rock/class.hh"
#include "rock/file.hh"
#include "rock/init.hh"
#include "rock/lexer.hh"
#include "rock/object.hh"
#include "rock/parser.hh"
#include "rock/scope.hh"
