#ifndef rock_core_init_hh
#define rock_core_init_hh

#include <string>

namespace rock {

using Runnable = void(*)();

void initialize();
void finalize();

class Init final {
public:
  Init(int, const std::string&, Runnable);
  Init(int, const std::string&, Runnable, Runnable);
};

}

#endif//rock_core_init_hh
