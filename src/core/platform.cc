#include "rock/core/all.hh"

#include <string>

namespace rock {

Platform *platform;
Class *classPlatform;

namespace {
Init init(110, __FILE__, []() {
  classPlatform = new Class("__Platform", {classObject}, true, {}, {
    {"getName", [](Reference owner, const Args &args) {
      checkargs(0, args);
      return String::from(platform->getName());
    }},
  });
  builtins->declare("__Platform", classPlatform);

  builtins->declare("__platform", new UserObject(classPlatform));
});
}

Platform::~Platform() {}

std::string Platform::getName() const {
  return "core";
}

char Platform::getFileSeparator() const {
  return '/';
}

char Platform::getPathSeparator() const {
  return ':';
}

}
