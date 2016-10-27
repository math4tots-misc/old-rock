#ifndef rock_eval_hh
#define rock_eval_hh

#include <string>

namespace rock {

struct Scope;
struct Result;

Result eval(Scope* scope, const std::string& code);
Result eval(const std::string& code);

}

#endif//rock_eval_hh
