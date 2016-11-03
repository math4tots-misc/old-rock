.PHONY: all clean test

CXX = g++
CXX_FLAGS = -Iinclude -Ibin/include --std=c++11 -Wall -Werror  # -O3 -flto

ALL_HH = bin/include/rock/all.hh
INCLUDES = $(wildcard include/rock/*.hh) $(ALL_HH)
TEST_SOURCES = $(wildcard test/*.cc)
TEST_SCRIPTS = $(wildcard test/*.rock)
SOURCES = $(wildcard src/*.cc)
OBJECTS = $(patsubst %.cc,%.o,$(patsubst src/%,bin/%,$(SOURCES)))
TESTBINS = $(patsubst %.cc,%.test,$(patsubst test/%,bin/%,$(TEST_SOURCES)))
TESTS = $(patsubst bin/%,%,$(TESTBINS))
SCRIPT_TESTS = $(patsubst test/%.rock,%.rock_test,$(TEST_SCRIPTS))

# Keep object files around for faster compilation.
# By default, make won't keep these files because they are 'intermediate'.
.SECONDARY: $(OBJECTS) $(TESTBINS)

all: test bin/rock

test: $(TESTS) $(SCRIPT_TESTS)

bin/rock: ccbin/rock.cc $(OBJECTS) $(INCLUDES)
	$(CXX) $(CXX_FLAGS) $(OBJECTS) ccbin/rock.cc -o bin/rock

%.rock_test: test/%.rock bin/rock
	bin/rock ./$<

%.test: bin/%.test
	./$<

bin/include/rock/all.hh: $(wildcard include/rock/*.hh)
	mkdir -p bin/include/rock
	ls include/rock/ | awk '{ print "#include \"rock/"$$0"\"" }' > $(ALL_HH)

bin/%.test: test/%.cc $(OBJECTS) $(INCLUDES)
	$(CXX) $(CXX_FLAGS) $(OBJECTS) $< -o $@

bin/%.o: src/%.cc $(INCLUDES)
	$(CXX) -c $(CXX_FLAGS) $< -o $@

clean:
	rm -rf bin
