#ifndef rock_parse_hh
#define rock_parse_hh

#include <memory>
#include <vector>

namespace rock {

class Ast;
class File;

class Unit final {
public:
  const std::unique_ptr<File> file;
  const std::unique_ptr<Ast> node;
  Unit(std::unique_ptr<File>, std::unique_ptr<Ast>);
};

std::unique_ptr<Unit> parseFile(std::unique_ptr<File>);
std::unique_ptr<Unit> parseFile(const std::string&, const std::string&);
std::unique_ptr<Unit> parseModule(const std::string&);
std::unique_ptr<Unit> parseExpression(const std::string&);

}

#endif//rock_parse_hh
