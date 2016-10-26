#ifndef rock_reference_hh
#define rock_reference_hh

#include <vector>

namespace rock {

struct Reference;
struct Result;
struct Value;
using Args = std::vector<Reference>;

/** Reference is basically a pointer to a Value with reference counting.
 * The main way to interact with a reference is by calling methods on it
 * using the 'call' method.
 * The other main thing you can do is to directly access the Value pointer.
 */
struct Reference final {
  Value *pointer;
  Reference();  // NOTE: sets pointer to 'nil', NOT 'nullptr'
  Reference(Value *p): pointer(p) { acquire(pointer); }
  Reference(const Reference& r): Reference(r.pointer) {}
  ~Reference() { release(pointer); }
  Reference& operator=(const Reference& r) {
    acquire(r.pointer);
    release(pointer);
    pointer = r.pointer;
    return *this;
  }
  Result call(const std::string& name, const Args& args) const;
  std::string debugstr() const;
private:
  static void acquire(Value*);
  static void release(Value*);
};

}

#endif//rock_reference_hh
