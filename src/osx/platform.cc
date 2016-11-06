#include "rock/osx/all.hh"

namespace rock {

namespace {
Init init(1050, __FILE__, []() {
  // TODO: It's bad form deleting something and reassigning.
  // Makes bugs harder to track and where things come from.
  // Figure out something better.
  delete platform;
  platform = new OsxPlatform();
});
}

std::string OsxPlatform::getName() const {
  return "OSX";
}

}
