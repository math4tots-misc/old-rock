#ifndef rock_check_hh
#define rock_check_hh

#include <vector>

namespace rock {

class Reference;
class Class;

using Args = std::vector<Reference>;

void checkargs(int, const Args&);
void checkargs(int, int, const Args&);
void checkargsmin(int, const Args&);
void checktype(Class*, Reference);

}

#endif//rock_check_hh
