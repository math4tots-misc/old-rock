#ifndef rock_base_result_hh
#define rock_base_result_hh

namespace rock {

constexpr short NORMAL = 0;
constexpr short EXCEPTION = 1;
constexpr short RETURN = 2;
constexpr short BREAK = 3;
constexpr short CONTINUE = 4;

/** General Result class for returning values with context.
 */
template <class T>
struct Result final {
  const short type;
  T value;
  Result(short t, T v): type(t), value(v) {}
};

}

#endif//rock_base_result_hh
