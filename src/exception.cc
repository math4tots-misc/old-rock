#include "rock/all.hh"

#include <sstream>

namespace rock {

Class *classException;

namespace {
Init init(110, __FILE__, []() {
  classException = new Class("Exception", {classObject}, {});
  builtins->declare("Exception", classException);
});

std::vector<const Token*> getStackTrace() {
  std::vector<const Token*> trace;
  for (const Token *t: *stack) {
    trace.push_back(t);
  }
  return trace;
}
}

Exception::Exception(const std::string &m):
    message(m), stacktrace(getStackTrace()) {}

Reference Exception::getClass() const {
  return classException;
}

std::string Exception::debug() const {
  return "<Exception: " + message + ">";
}

std::string Exception::str() const {
  std::stringstream ss;
  ss << "\n####################";
  ss << "\n## " << message;
  ss << "\n####################";
  for (const Token *t: stacktrace) {
    ss << t->locationMessage();
  }
  return ss.str();
}

}
