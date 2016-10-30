#ifndef rock_parse_hh
#define rock_parse_hh

#include <vector>
#include <memory>

namespace rock {

class Ast;
class File;

std::unique_ptr<Ast> parseFile(const File&);
std::unique_ptr<Ast> parseFile(const std::string&, const std::string&);
std::unique_ptr<Ast> parseModule(const std::string&);
std::unique_ptr<Ast> parseExpression(const std::string&);

}

#endif//rock_parse_hh
