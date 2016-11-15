#ifndef rock_core_token_hh
#define rock_core_token_hh

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
  std::string locationMessage() const;
  long getLineNumber() const;
  long getColumnNumber() const;
  std::string getLine() const;
  std::string compactLocationMessage() const;
};

}

#endif//rock_core_token_hh
