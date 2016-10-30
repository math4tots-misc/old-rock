#ifndef rock_parse_hh
#define rock_parse_hh

#include <memory>
#include <vector>

namespace rock {

class Ast;
class File;

class Unit final {
public:
  std::unique_ptr<File> file;
  const Ast *node;  // TODO: Use unique_ptr<Ast>, see ast.hh
  Unit(std::unique_ptr<File>, Ast*);
};

std::unique_ptr<Unit> parseFile(std::unique_ptr<File>);
std::unique_ptr<Unit> parseFile(const std::string&, const std::string&);
std::unique_ptr<Unit> parseModule(const std::string&);
std::unique_ptr<Unit> parseExpression(const std::string&);

}

#endif//rock_parse_hh
