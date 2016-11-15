#ifndef rock_core_file_hh
#define rock_core_file_hh

#include <string>

namespace rock {

class File final {
public:
  const std::string path;
  const std::string contents;
  File(const std::string&, const std::string&);
};

}

#endif//rock_core_file_hh
