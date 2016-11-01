.PHONY: clean run test

CXX = g++
CXX_FLAGS = -Iinclude -Ibin/include --std=c++11 -Wall -Werror

ALL_HH = bin/include/rock/all.hh
INCLUDES = $(wildcard include/rock/*.hh) $(ALL_HH)
TEST_SOURCES = $(wildcard test/*.cc)
SOURCES = $(wildcard src/*.cc)
OBJECTS = $(patsubst %.cc,%.o,$(patsubst src/%,bin/%,$(SOURCES)))
TESTBINS = $(patsubst %.cc,%.test,$(patsubst test/%,bin/%,$(TEST_SOURCES)))
TESTS = $(patsubst bin/%,%,$(TESTBINS))

# Keep object files around for faster compilation.
# By default, make won't keep these files because they are 'intermediate'.
.SECONDARY: $(OBJECTS) $(TESTBINS)

test: $(TESTS)

bin/rock: ccbin/rock.cc $(OBJECTS) $(INCLUDES)
	$(CXX) $(CXX_FLAGS) $(OBJECTS) ccbin/rock.cc -o bin/rock

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
