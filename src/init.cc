#include "rock/all.hh"

#include <vector>
#include <functional>
#include <map>

namespace rock {

namespace {
std::map<int, std::vector<std::function<void()>>>* inits;

std::map<int, std::vector<std::function<void()>>>& getInits() {
  if (!inits) {
    inits = new std::map<int, std::vector<std::function<void()>>>();
  }
  return *inits;
}
}

void init() {
  for (auto pair: getInits()) {
    for (auto f: pair.second) {
      f();
    }
  }
}

Init::Init(int priority, std::function<void()> f) {
  getInits()[priority].push_back(f);
}

}
