#ifndef rock_file_hh
#define rock_file_hh

#include <string>

namespace rock {

struct File {
  const std::string file_name;
  const std::string contents;
  File(const std::string& fn, const std::string& c):
      file_name(fn), contents(c) {}
};

struct Token {
  const File *const file;
  const long position;
  Token(const File * f, long p): file(f), position(p) {}
};

}

#endif//rock_file_hh
