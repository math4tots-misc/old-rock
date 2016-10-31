.PHONY: clean run test

CXX = g++
CXX_FLAGS = -Iinclude --std=c++11 -Wall -Werror

INCLUDES = $(wildcard include/rock/*.hh)
TEST_SOURCES = $(wildcard test/*.cc)
SOURCES = $(wildcard src/*.cc)
OBJECTS = $(patsubst %.cc,%.o,$(patsubst src/%,bin/%,$(SOURCES)))
TESTBINS = $(patsubst %.cc,%.test,$(patsubst test/%,bin/%,$(TEST_SOURCES)))
TESTS = $(patsubst bin/%,%,$(TESTBINS))

# Keep object files around for faster compilation.
# By default, make won't keep these files because they are 'intermediate'.
.SECONDARY: $(OBJECTS) $(TESTBINS)

test: $(TESTS)

%.test: bin/%.test
	./$<

bin/%.test: test/%.cc $(OBJECTS) $(INCLUDES)
	mkdir -p bin && $(CXX) $(CXX_FLAGS) $(OBJECTS) $< -o $@

bin/%.o: src/%.cc $(INCLUDES)
	mkdir -p bin && $(CXX) -c $(CXX_FLAGS) $< -o $@

clean:
	rm -rf bin
