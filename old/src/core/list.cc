#include "rock/core/all.hh"

#include <sstream>

namespace rock {

Class *classList;
Class *classListClass;

namespace {
Init init(110, __FILE__, []() {
  classListClass = new Class("ListClass", {classClass}, {
    {"fromIterable", [](Reference, Class*, const Args &args) {
      checkargs(1, args);
      Reference iter = args[0]->call("__iter", {});
      std::vector<Reference> items;
      while (iter->call("more", {})->truthy()) {
        items.push_back(iter->call("next", {}));
      }
      return new List(items);
    }},
  });
  builtins->declare("ListClass", classListClass);

  classList = new Class(classListClass, "List", {classObject}, {
    {"__str", [](Reference owner, Class*, const Args &args) {
      checkargs(0, args);
      return String::from(owner->str());
    }},
    {"push", [](Reference owner, Class*, const Args &args) {
      checkargs(1, args);
      owner.as<List>()->value.push_back(args[0]);
      return owner;
    }},
    {"__eq", [](Reference owner, Class*, const Args &args) -> Reference {
      checkargs(1, args);
      if (!instanceof(args[0], classList)) { return xfalse; }
      return Bool::from(owner.as<List>()->value == args[0].as<List>()->value);
    }},
  });
  builtins->declare("List", classList);
});
}

List::List(const std::vector<Reference>& v): value(v) {}

Reference List::getClass() const {
  return classList;
}

std::string List::debug() const {
  std::stringstream ss;
  ss << "L(";
  bool first = true;
  for (const Reference& r: value) {
    if (!first) {
      ss << ", ";
    }
    ss << r->debug();
    first = false;
  }
  ss << ")";
  return ss.str();
}

std::string List::str() const {
  std::stringstream ss;
  ss << "L(";
  bool first = true;
  for (const Reference& r: value) {
    if (!first) {
      ss << ", ";
    }
    ss << r->str();
    first = false;
  }
  ss << ")";
  return ss.str();
}

}
