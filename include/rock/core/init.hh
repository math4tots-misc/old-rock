#ifndef rock_init_hh
#define rock_init_hh

#include <string>

namespace rock {

void initialize();
void finalize();

class Init final {
public:
  Init(int, const std::string&, std::function<void()>);
  Init(int, const std::string&, std::function<void()>, std::function<void()>);
};

}

#endif//rock_init_hh
