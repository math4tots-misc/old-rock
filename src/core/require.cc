#include "rock/core/all.hh"

#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>

namespace rock {

namespace {

bool fileExists(const std::string &uri) {
  return std::ifstream(uri).good();
}

std::string slurp(const std::string &filename) {
  std::ifstream fin(filename);
  return std::string(
    (std::istreambuf_iterator<char>(fin)),
    std::istreambuf_iterator<char>());
}

Reference load(const std::string &uri) {
  std::stringstream ss;
  ss.str(std::getenv("ROCKPATH"));
  std::string dir;
  bool found = false;
  while (std::getline(ss, dir, ':')) {
    if (dir.empty()) { continue; }
    if (dir.back() != '/') {
      dir.push_back('/');
    }
    if (fileExists(dir + uri)) {
      found = true;
      break;
    }
  }
  if (!found) {
    throw Reference(new Exception("No such module: " + uri));
  }
  std::string contents = slurp(dir + uri);
  Block *block = parseFile(uri, contents);
  return moduleFrom(uri, block);
}

void declareFunc(
    Scope *scope, const std::string &name,
    Reference(*f)(const Args&)) {
  scope->declare(name, Function::from(name, f));
}

Init init(110, __FILE__, []() {
  declareFunc(builtins, "require", [](const Args &args) {
    checkargs(1, args);
    checktype(classString, args[0]);
    return require(args[0]->str());
  });
});

}  // namespace

Reference require(const std::string &path) {
  if (moduleRegistry->find(path) == moduleRegistry->end()) {
    (*moduleRegistry)[path] = load(path);
  }
  return (*moduleRegistry)[path];
}

}
