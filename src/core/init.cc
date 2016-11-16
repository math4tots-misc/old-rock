#include "rock/core/all.hh"

#include <vector>
#include <map>

//// For debugging
// #include <iostream>

namespace rock {

namespace {
std::map<int,std::vector<std::pair<std::string,Runnable>>> *inits;

std::map<int,std::vector<std::pair<std::string,Runnable>>> *fins;

std::map<int,std::vector<std::pair<std::string,Runnable>>> &getInits() {
  if (!inits) {
    inits = new std::map<int,std::vector<std::pair<std::string, Runnable>>>();
  }
  return *inits;
}


std::map<int,std::vector<std::pair<std::string,Runnable>>> &getFins() {
  if (!fins) {
    fins = new std::map<int,std::vector<std::pair<std::string, Runnable>>>();
  }
  return *fins;
}
}  // namespace

void initialize() {
  // std::cerr << "Starting initialization process" << std::endl;
  for (auto entry: getInits()) {
    // std::cerr << "Initializing priority: " << entry.first << std::endl;
    for (auto pair: entry.second) {
      // std::cerr << "Initializing: " << pair.first << std::endl;
      pair.second();
    }
  }
}

void finalize() {
  // std::cerr << "Starting finailization process" << std::endl;
  auto& fins = getFins();
  // std::cerr << "Got fins" << std::endl;
  for (auto ep = fins.rbegin(); ep != fins.rend(); ++ep) {
    // std::cerr << "Finalizing priority: " << ep->first << std::endl;
    auto& ff = ep->second;
    for (auto pp = ff.rbegin(); pp != ff.rend(); ++pp) {
      // std::cerr << "Finalizing: " << pp->first << std::endl;
      pp->second();
    }
  }
}

/**
 * 'priority' is the number that determines when an initializer will
 * get run. Lower numbers run first.
 * 'tag' is a name for the initializer for debugging purposes.
 * 'f' is the initializer function to run.
 */
Init::Init(int priority, const std::string &tag, Runnable f):
    Init(priority, tag, f, [](){}) {}

Init::Init(
    int priority, const std::string &tag,
    Runnable init, Runnable fin) {
  // std::cerr << "Setting initializer for " <<
  // priority << " " << tag << std::endl;
  getInits()[priority].push_back(std::make_pair(tag, init));
  getFins()[priority].push_back(std::make_pair(tag, fin));
}
}  // namespace rock
