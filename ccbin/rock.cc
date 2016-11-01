#include "rock/all.hh"

#include <fstream>
#include <iostream>
#include <streambuf>
#include <string>

using namespace std;
using namespace rock;

string load(const string &filename) {
  ifstream fin(filename);
  return string(
    (istreambuf_iterator<char>(fin)),
    istreambuf_iterator<char>());
}

int main(int argc, char **argv) {
  if (argc != 2) {
    cout << "usage: " << argv[0] << " script.rock" << endl;
  } else {
    initialize();
    unique_ptr<Unit> unit = parseFile(argv[1], load(argv[1]));
    Scope scope(builtins);
    try {
      Result result = unit->node->eval(scope);
      if (result.type != Result::Type::OK) {
        cerr << "Uncaught exception (by value): ";
        if (result.value.operator->()) {
          cerr << result.value->str() << endl;
        } else {
          cerr << result.debug() << endl;
        }
      }
    } catch (const char *str) {
      cerr << "Uncaught exception (const char*): " << str << endl;
    } catch (const Reference &r) {
      if (r.operator->()) {
        cerr << "Uncaught exception (Reference): " << r->str() << endl;
      } else {
        cerr << "Uncaught exception (null Reference)" << endl;
      }
    }
    finalize();
  }
}
