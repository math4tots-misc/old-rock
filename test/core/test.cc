// TODO: Factor out tests

#include "rock/core/all.hh"
#include <iostream>
#include <memory>
#include <cassert>
using namespace std;
using namespace rock;

int main() {
  try {
    rock::initialize();

    assert(classClass->debug() == "<Class Class>");
    assert(classException->debug() == "<Class Exception>");

    File f("a", "b");
    File g(f);
    {
      Ast *node = parseFile("<test>", "a + b");
      assert(node->debug() == "Block(N4rock10MethodCallE,)");
      Reference scope(new Scope());
      bool caught = false;
      try {
        node->eval(*scope.as<Scope>())->debug();
      } catch (const Reference &r) {
        caught = true;
        assert(r->debug() == "<Exception: No such variable (get): 'a'>");
      }
      if (!caught) {
        assert(false);  // no exception was thrown
      }
    }
    {
      Ast *node = parseFile("<test>", "String");
      assert(node->debug() == "Block(N4rock4NameE,)");
      Reference scope(new Scope(builtins));
      assert(node->eval(*scope.as<Scope>())->debug() == "<Class String>");
    }
    {
      Ast *node = parseFile("<test>", "'hello world!'");
      assert(node->debug() == "Block(N4rock7LiteralE,)");
      Reference scope(new Scope(builtins));
      assert(
          node->eval(*scope.as<Scope>())->debug() == "String(hello world!)");
    }

    {
      Ast *node = parseFile("<test>", "L(1.0,2,3).__str()");
      assert(node->debug() == "Block(N4rock10MethodCallE,)");
      Reference scope(new Scope(builtins));
      Reference result = node->eval(*scope.as<Scope>());
      assert(result->str() == "L(1, 2, 3)");
    }
    rock::finalize();
  } catch (const ParseError &pe) {
    std::cerr << pe.str() << std::endl;
    return 1;
  } catch (const Reference &r) {
    std::cerr << r->str() << std::endl;
    return 1;
  }
}
