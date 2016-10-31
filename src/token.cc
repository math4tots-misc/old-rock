#include "rock/all.hh"

namespace rock {

Token::Token(
      const File &f, long p,
      const std::string &t, const std::string &v):
          file(f), position(p), type(t), value(v) {}

}
