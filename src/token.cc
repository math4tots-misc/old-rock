#include "rock/all.hh"

#include <iostream>
#include <sstream>
#include <algorithm>

namespace rock {

Token::Token(
      const File &f, long p,
      const std::string &t, const std::string &v):
          file(f), position(p), type(t), value(v) {}

std::string Token::locationMessage() const {
  std::stringstream ss;
  ss << "\nOn line " << getLineNumber()
      << " in file " << file.path << std::endl
      << getLine() << std::endl;
  long columnNumber = getColumnNumber();
  for (long i = 1; i < columnNumber; i++) {
    ss << " ";
  }
  ss << "*";
  return ss.str();
}

long Token::getLineNumber() const {
  long lineNumber = 1;
  long limit = std::min<long>(file.contents.size(), position);
  for (long i = 0; i < limit; i++) {
    if (file.contents[i] == '\n') {
      lineNumber++;
    }
  }
  return lineNumber;
}

long Token::getColumnNumber() const {
  long columnNumber = 1;
  for (long i = std::min<long>(file.contents.size(), position); i > 0; i--) {
    if (file.contents[i-1] == '\n') {
      break;
    }
    columnNumber++;
  }
  return columnNumber;
}

std::string Token::getLine() const {
  long start = position;
  long end = position;
  while (start > 0 && file.contents[start-1] != '\n') {
    start--;
  }
  while (end < file.contents.size() && file.contents[end] != '\n') {
    end++;
  }
  return file.contents.substr(start, end - start);
}

}
