# Rock programming language

## Setup

You should set the `ROCKPATH` variable.

```sh
export ROCKPATH="$ROCKPATH:$HOME/git/rock/modules"
```

## style

Implementations should not live in headers unless they are templates.

## Build process

Can be verified in Makefile. But still, here's an overview in case you don't have GNU make and other \*NIX friends:

The `src/` directory contains all the C++ files you need for building `rock`.

The `include/rock` directory contains almost all the header files you need, except for `rock/*/all.hh`. Since it's such a hassle to maintain a list of all headers by hand, the makefile generates one in `bin/include/rock/*/all.hh` by running:

    ls include/rock/<dir> | awk '{ print "#include \"rock/<dir>/"$0"\"" }' > bin/include/rock/<dir>/all.hh

That's basically it. Then you just run your compiler with all the C++ files in `src/` with the include directories `include/` and `bin/include/`.

## Error handling

Use underlying C++ exceptions.

## Init

Since C++ initialization order is indeterminate, I use a custom initializer. See `include/core/init.hh`.

The way initialization works, is that everyone has an initialization level. Lower levels are initialized earlier, and deinitialized later.

Current levels:

  * 50
    - The `builtins` Scope variable is instantiated here.
  * 100
    - The `stack` is initialized here.
    - The `classObject` variable is initialized here.
  * 110
    - Most `core` class objects are initialized here.
    - `moduleRegistry` is initialized here.
  * 120
    - `builtins` is populated here (e.g. `print` function)
  * 1050
    - The `platform` variable is reset to something more specific here
      e.g. `OSX`.
  * 2000
    - `core/__prelude.rock` is called with the `builtin` Scope.

## core/ and osx/

The plan is that all platforms will benefit from code in `core/`. This includes everything in `include/rock/core`, `ccbin/core`, `src/core`, `test/core`.

Stuff like `osx/` are extensions specific to OS X.

## Special methods

Some special methods are explicitly denoted as special to the language by starting with two underscores (e.g. `__init`).

However, there are methods I give significance to, even though they might not look that way. These really should be interfaces.

  - Functors and monads
    - `map(f)`
    - `flatMap(f)`
    - `each(f)`
  - Collections
    - `iter()`
    - `size`
