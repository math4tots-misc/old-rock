#ifndef rock_file_hh
#define rock_file_hh

#include <string>

namespace rock {

class File final {
public:
  const std::string path;
  const std::string contents;
  File(const std::string&, const std::string&);
};

class Token final {
public:
  const File &file;
  const long position;
  const std::string type, value;
  Token(const File&, long, const std::string&);
  Token(const File&, long, const std::string&, const std::string&);
};

}

#endif//rock_file_hh
