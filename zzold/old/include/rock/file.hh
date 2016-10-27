#ifndef rock_file_hh
#define rock_file_hh

#include <string>

namespace rock {

struct File {
  const std::string path, contents;
  File(const std::string& p, const std::string& c): path(p), contents(c) {}
};

struct Token {
  File *const file;
  const long position;
  const std::string type, value;
  Token(File *const f, long p, const std::string& t): Token(f, p, t, "") {}
  Token(File *const f, long p, const std::string& t, const std::string& v):
      file(f), position(p), type(t), value(v) {}
};

}

#endif//rock_file_hh
