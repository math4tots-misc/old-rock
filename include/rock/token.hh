#ifndef rock_token_hh
#define rock_token_hh

#include <string>

namespace rock {

class File;

class Token final {
public:
  const File &file;
  const long position;
  const std::string type;
  const std::string value;
  Token(const File&, long, const std::string&);
  Token(const File&, long, const std::string&, const std::string&);
};

}

#endif//rock_token_hh