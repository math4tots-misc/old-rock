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
  int exitcode = 0;
  if (argc != 2) {
    cout << "usage: " << argv[0] << " script.rock" << endl;
  } else {
    initialize();
    unique_ptr<Unit> unit = parseFile(argv[1], load(argv[1]));
    Reference scope(new Scope(builtins));
    try {
      Result result = unit->node->eval(*scope.as<Scope>());
      if (result.type != Result::Type::OK) {
        cerr << "Uncaught exception (Result): ";
        if (result.value.operator->()) {
          cerr << result.value->str() << endl;
        } else {
          cerr << "(empty)" << result.debug() << endl;
        }
        exitcode = 1;
      }
    } catch (const char *str) {
      cerr << "Uncaught exception (const char*): " << str << endl;
      exitcode = 1;
    } catch (const Reference &r) {
      if (r.operator->()) {
        cerr << "Uncaught exception (Reference): " << r->str() << endl;
      } else {
        cerr << "Uncaught exception (null Reference)" << endl;
      }
      exitcode = 1;
    }
    finalize();
  }
  return exitcode;
}
