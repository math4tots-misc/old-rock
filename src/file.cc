#include "rock/all.hh"

#include <map>

namespace rock {

File::File(const std::string &p, const std::string &c):
    path(p), contents(c) {}

Token::Token(const File &f, long p, const std::string &t):
    file(f), position(p), type(t) {}

}
