// g++ foo.cc src/rock.cc --std=c++11 -Iinclude -Wall -Werror -Wpedantic && ./a.out
#include <iostream>
#include "rock.hh"
using namespace std;
using namespace rock;

int main() {
  Ast *literal = new Literal(nullptr, new String("Foo"));
  Result result = literal->eval(nullptr);
  cout << (result.type == NORMAL) << endl;
}
