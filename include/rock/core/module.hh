#ifndef rock_core_module_hh
#define rock_core_module_hh

#include <string>

namespace rock {

class Reference;
class Block;

// Given a parsed Block, return a module object.
// Module objects are actually UserObjects with a custom Class
// that has a field and method for every declared variable in the file.
// All fields in a module object are readonly.
Reference moduleFrom(const std::string&, Block*);

}

#endif//rock_core_module_hh
