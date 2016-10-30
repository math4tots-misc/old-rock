#include "rock/all.hh"

#include <vector>
#include <functional>
#include <map>

namespace rock {

namespace {

Reference *brp;  // pointer to builtins reference

Init init(5, __FILE__, []() {
  builtins = new Scope();
  brp = new Reference(builtins);  // live until explicitly deleted
}, []() {
  delete brp;
});
}

Scope *builtins;

}
