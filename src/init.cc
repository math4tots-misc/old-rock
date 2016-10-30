#include "rock/all.hh"

#include <vector>
#include <functional>
#include <map>

namespace rock {

namespace {
std::map<int,std::vector<std::pair<std::string,std::function<void()>>>>*
inits;

std::map<int,std::vector<std::pair<std::string,std::function<void()>>>>&
getInits() {
  if (!inits) {
    inits = new std::map<
        int,std::vector<
            std::pair<
                std::string,
                std::function<void()>>>>();
  }
  return *inits;
}
}  // namespace

void init() {
  for (auto entry: getInits()) {
    for (auto pair: entry.second) {
      pair.second();
    }
  }
}

/**
 * 'priority' is the number that determines when an initializer will
 * get run. Lower numbers run first.
 * 'tag' is a name for the initializer for debugging purposes.
 * 'f' is the initializer function to run.
 */
Init::Init(int priority, const std::string &tag, std::function<void()> f) {
  getInits()[priority].push_back(std::make_pair(tag, f));
}

}  // namespace rock
