#include "rock/all.hh"

#include <vector>
#include <functional>
#include <map>

namespace rock {

namespace {
Init init(5, __FILE__, []() {
  builtins = new Scope();
}, []() {
  delete builtins;
});
}

Scope *builtins;

}
