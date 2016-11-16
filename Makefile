.PHONY: clean all test

CXX = g++
CXX_FLAGS_COMMON = -Iinclude -Iout/include --std=c++11 -Wall -Werror
CXX_FLAGS_DEV = $(CXX_FLAGS_COMMON) -O0 -g
CXX_FLAGS_PROD = $(CXX_FLAGS_COMMON) -O3 -flto
CXX_FLAGS = $(CXX_FLAGS_DEV)

ALL_HH = out/include/rock/all.hh
XINCLUDES = $(wildcard include/rock/*.hh)
INCLUDES = $(XINCLUDES) $(ALL_HH)
SOURCES = $(wildcard src/*.cc)
TEST_SOURCES = $(wildcard test/*.cc)
OBJECTS = $(patsubst %.cc,%.o,$(patsubst src/%,out/objects/%,$(SOURCES)))
TESTBINS = $(patsubst %.cc,%.test,$(patsubst test/%,out/bin/%,$(TEST_SOURCES)))
CC_TESTS = $(patsubst out/bin/%,%,$(TESTBINS))

.SECONDARY: $(OBJECTS) $(TESTBINS)

test: $(CC_TESTS)

%.test: out/bin/%.test
	./$<

out/bin/%.test: test/%.cc $(OBJECTS) | out/bin
	$(CXX) $(CXX_FLAGS) $(OBJECTS) $< -o $@

out/bin/rock.out: bin/rock.cc $(OBJECTS) | out/bin
	$(CXX) $(CXX_FLAGS) $(OBJECTS) bin/rock.cc -o out/rock

$(ALL_HH): $(XINCLUDES) | out/include/rock
	ls include/rock | awk '{ print "#include \"rock/"$$0"\"" }' > $(ALL_HH)

out/objects/%.o: src/%.cc $(INCLUDES) | out/objects
	$(CXX) -c $(CXX_FLAGS) $< -o $@

out/bin:
	mkdir -p out/bin

out/objects:
	mkdir -p out/objects

out/include/rock:
	mkdir -p out/include/rock

clean:
	rm -rf out
