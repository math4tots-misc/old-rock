#include "rock/eval.hh"
#include "rock/result.hh"
#include "rock/file.hh"
#include "rock/ast.hh"
#include "rock/parser.hh"
#include "rock/globals.hh"

namespace rock {

Result eval(Scope* scope, const std::string& code) {
  File *file = new File("<eval>", code);
  Ast *node = parse("Expression", file);
  Result result = node->eval(scope);
  return result;
}

Result eval(const std::string& code) {
  Scope *scope = makeGlobals();
  Result result = eval(scope, code);
  delete scope;
  return result;
}

}
