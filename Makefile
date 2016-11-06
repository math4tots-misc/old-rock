.PHONY: clean core_all core_test osx_all osx_test

# TODO: Add '-O3 -flto' to CXX_FLAGS

CORE_CXX = g++
CORE_CXX_FLAGS = -Iinclude -Ibin/include --std=c++11 -Wall -Werror
CORE_ALL_HH = bin/include/rock/core/all.hh
CORE_XINCLUDES = $(wildcard include/rock/core/*.hh)
CORE_INCLUDES = $(CORE_XINCLUDES) $(CORE_ALL_HH)
CORE_SOURCES = $(wildcard src/core/*.cc)
CORE_TEST_SOURCES = $(wildcard test/core/*.cc)
CORE_TEST_SCRIPTS = $(wildcard test/core/*.rock)
CORE_OBJECTS = $(patsubst %.cc,%.o,$(patsubst src/%,bin/%,$(CORE_SOURCES)))
CORE_TESTBINS = $(patsubst %.cc,%.test,$(patsubst test/%,bin/%,$(CORE_TEST_SOURCES)))
CORE_CC_TESTS = $(patsubst bin/%,%,$(CORE_TESTBINS))
CORE_SCRIPT_TESTS = $(patsubst test/%.rock,%.rock_test,$(CORE_TEST_SCRIPTS))
CORE_TESTS = $(CORE_CC_TESTS) $(CORE_SCRIPT_TESTS)

OSX_CXX = g++
OSX_CXX_FLAGS = -Iinclude -Ibin/include --std=c++11 -Wall -Werror
OSX_ALL_HH = bin/include/rock/osx/all.hh
OSX_XINCLUDES = $(wildcard include/rock/osx/*.hh)
OSX_INCLUDES = $(OSX_XINCLUDES) $(OSX_ALL_HH) $(CORE_INCLUDES)
OSX_SOURCES = $(wildcard src/osx/*.cc) $(CORE_SOURCES)
OSX_TEST_SOURCES = $(wildcard test/osx/*.cc)
OSX_TEST_SCRIPTS = $(wildcard test/osx/*.rock)
OSX_OBJECTS = $(patsubst %.cc,%.o,$(patsubst src/%,bin/%,$(OSX_SOURCES)))
OSX_TESTBINS = $(patsubst %.cc,%.test,$(patsubst test/%,bin/%,$(OSX_TEST_SOURCES)))
OSX_CC_TESTS = $(patsubst bin/%,%,$(OSX_TESTBINS))
OSX_SCRIPT_TESTS = $(patsubst test/%.rock,%.rock_test,$(OSX_TEST_SCRIPTS))
OSX_TESTS = $(OSX_CC_TESTS) $(OSX_SCRIPT_TESTS)

# Keep object files around for faster compilation.
# By default, make won't keep these files because they are 'intermediate'.
.SECONDARY: $(CORE_OBJECTS) $(CORE_TESTBINS) $(OSX_OBJECTS) $(OSX_TESTBINS)

## OS X

osx_all: osx_test

osx_test: $(OSX_CC_TESTS) $(OSX_SCRIPT_TESTS) core_test

bin/osx/rock: ccbin/osx/rock.cc $(OSX_OBJECTS) $(OSX_INCLUDES)
	$(OSX_CXX) $(OSX_CXX_FLAGS) $(OSX_OBJECTS) ccbin/osx/rock.cc -o bin/osx/rock

osx/%.rock_test: test/osx/%.rock bin/osx/rock
	bin/osx/rock ./$<

osx/%.test: bin/osx/%.test
	./$<

$(OSX_ALL_HH): $(OSX_XINCLUDES)
	mkdir -p bin/include/rock/osx
	ls include/rock/osx | awk '{ print "#include \"rock/osx/"$$0"\"" }' > $(OSX_ALL_HH)

bin/osx/%.test: test/osx/%.cc $(OSX_OBJECTS) $(OSX_INCLUDES)
	$(OSX_CXX) $(OSX_CXX_FLAGS) $(OSX_OBJECTS) $< -o $@

bin/osx/%.o: src/osx/%.cc $(OSX_INCLUDES) $(CORE_INCLUDES) | bin/osx
	$(OSX_CXX) -c $(OSX_CXX_FLAGS) $< -o $@

bin/osx:
	mkdir -p bin/osx


## core

core_all: core_test

core_test: $(CORE_CC_TESTS) $(CORE_SCRIPT_TESTS)

bin/core/rock: ccbin/core/rock.cc $(CORE_OBJECTS) $(CORE_INCLUDES)
	$(CORE_CXX) $(CORE_CXX_FLAGS) $(CORE_OBJECTS) ccbin/core/rock.cc -o bin/core/rock

core/%.rock_test: test/core/%.rock bin/core/rock
	bin/core/rock ./$<

core/%.test: bin/core/%.test
	./$<

$(CORE_ALL_HH): $(CORE_XINCLUDES)
	mkdir -p bin/include/rock/core
	ls include/rock/core | awk '{ print "#include \"rock/core/"$$0"\"" }' > $(CORE_ALL_HH)

bin/core/%.test: test/core/%.cc $(CORE_OBJECTS) $(CORE_INCLUDES)
	$(CORE_CXX) $(CORE_CXX_FLAGS) $(CORE_OBJECTS) $< -o $@

bin/core/%.o: src/core/%.cc $(CORE_INCLUDES) | bin/core
	$(CORE_CXX) -c $(CORE_CXX_FLAGS) $< -o $@

bin/core:
	mkdir -p bin/core

clean:
	rm -rf bin
