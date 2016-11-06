#ifndef rock_parse_hh
#define rock_parse_hh

#include <memory>
#include <vector>

namespace rock {

class Ast;
class File;

Block *parseFile(File*);

// The following files leak memory a File object
// unless you manually extract the File object associated
// with the Ast and delete it.
Block *parseFile(const std::string&, const std::string&);
Block *parseModule(const std::string&);
Ast *parseExpression(const std::string&);

}

#endif//rock_parse_hh
