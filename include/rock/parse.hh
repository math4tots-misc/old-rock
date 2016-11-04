#ifndef rock_parse_hh
#define rock_parse_hh

#include <memory>
#include <vector>

namespace rock {

class Ast;
class File;

class Unit final {
public:
  File *file;
  const Ast *node;  // TODO: Use unique_ptr<Ast>, see ast.hh
  Unit(File*, Ast*);
};

Unit *parseFile(File*);
Unit *parseFile(const std::string&, const std::string&);
Unit *parseModule(const std::string&);
Unit *parseExpression(const std::string&);

}

#endif//rock_parse_hh
