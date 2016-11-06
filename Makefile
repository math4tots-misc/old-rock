.PHONY: all clean test

CXX = g++
CXX_FLAGS = -Iinclude -Ibin/include --std=c++11 -Wall -Werror  # -O3 -flto

ALL_HH = bin/include/rock/core/all.hh
XINCLUDES = $(wildcard include/rock/core/*.hh)
INCLUDES = $(XINCLUDES) $(ALL_HH)
TEST_SOURCES = $(wildcard test/core/*.cc)
TEST_SCRIPTS = $(wildcard test/core/*.rock)
SOURCES = $(wildcard src/core/*.cc)
OBJECTS = $(patsubst %.cc,%.o,$(patsubst src/%,bin/%,$(SOURCES)))
TESTBINS = $(patsubst %.cc,%.test,$(patsubst test/%,bin/%,$(TEST_SOURCES)))
TESTS = $(patsubst bin/%,%,$(TESTBINS))
SCRIPT_TESTS = $(patsubst test/%.rock,%.rock_test,$(TEST_SCRIPTS))

# Keep object files around for faster compilation.
# By default, make won't keep these files because they are 'intermediate'.
.SECONDARY: $(OBJECTS) $(TESTBINS)

all: test bin/core/rock

test: $(TESTS) $(SCRIPT_TESTS)

bin/core/rock: ccbin/core/rock.cc $(OBJECTS) $(INCLUDES)
	$(CXX) $(CXX_FLAGS) $(OBJECTS) ccbin/core/rock.cc -o bin/core/rock

core/%.rock_test: test/core/%.rock bin/core/rock
	bin/core/rock ./$<

core/%.test: bin/core/%.test
	./$<

$(ALL_HH): $(XINCLUDES)
	mkdir -p bin/include/rock/core
	ls include/rock/core | awk '{ print "#include \"rock/core/"$$0"\"" }' > $(ALL_HH)

bin/core/%.test: test/core/%.cc $(OBJECTS) $(INCLUDES)
	$(CXX) $(CXX_FLAGS) $(OBJECTS) $< -o $@

bin/core/%.o: src/core/%.cc $(INCLUDES) | bin/core
	$(CXX) -c $(CXX_FLAGS) $< -o $@

bin/core:
	mkdir -p bin/core

clean:
	rm -rf bin
