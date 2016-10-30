#ifndef rock_init_hh
#define rock_init_hh

#include <string>

namespace rock {

void init();

class Init final {
public:
  Init(int, std::function<void()>);
};

}

#endif//rock_init_hh
