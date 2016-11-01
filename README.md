# Rock programming language

## style

Implementations should not live in headers unless they are templates.

## Build process

Can be verified in Makefile. But still, here's an overview in case you don't have GNU make and other \*NIX friends:

The `src/` directory contains all the C++ files you need for building `rock`.

The `include/rock` directory contains almost all the header files you need, except for `rock/all.hh`. Since it's such a hassle to maintain a list of all headers by hand, the makefile generates one in `bin/include/rock/all.hh` by running:

    ls include/rock/ | awk '{ print "#include \"rock/"$0"\"" }' > bin/include/rock/all.hh

That's basically it. Then you just run your compiler with all the C++ files in `src/` with the include directories `include/` and `bin/include/`.
