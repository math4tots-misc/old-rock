#include "rock/core/all.hh"

namespace rock {

void checkargs(int size, const Args &args) {
  if (args.size() != size) {
    throw Reference(new Exception(
        "Expected " + std::to_string(size) + " arguments, but got " +
        std::to_string(args.size())));
  }
}

void checkargs(int min, int max, const Args &args) {
  if (args.size() < min || args.size() > max) {
    throw Reference(new Exception(
        "Expected " + std::to_string(min) + " to " + std::to_string(max) +
        " arguments, but got " + std::to_string(args.size())));
  }
}

void checkargsmin(int min, const Args &args) {
  if (args.size() < min) {
    throw Reference(new Exception(
        "Expected at least " + std::to_string(min) +
        " arguments, but got " + std::to_string(args.size())));
  }
}

void checktype(Class *expected, Reference value) {
  Class *actual = value->getClass().as<Class>();
  if (expected != actual) {
    throw Reference(new Exception(
        "Expected type " + expected->name + " but got " + actual->name));
  }
}

void checkinstance(Class *expected, Reference value) {
  Class *actual = value->getClass().as<Class>();
  if (!actual->extends(expected)) {
    throw Reference(new Exception(
        "Expected subclass of " + expected->name +
        " but got " + actual->name));
  }
}

}


