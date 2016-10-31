.PHONY: clean run test

CXX = g++
CXX_FLAGS = -Iinclude --std=c++11 -Wall -Werror

SRC_DIR = src
BIN_DIR = bin
INCLUDES = $(wildcard include/rock/*.hh)
TEST_SOURCES = $(wildcard tests/*.cc)
SOURCES = $(wildcard src/*.cc)
OBJECTS = $(patsubst %.cc,%.o,$(patsubst src/%,bin/%,$(SOURCES)))
TESTS = $(patsubst %.cc,%.test,$(patsubst tests/%,bin/%,$(TEST_SOURCES)))

test: $(TESTS)
	for t in $(TESTS); do echo $$t && ./$$t; done

bin/%.test: tests/%.cc $(OBJECTS) $(INCLUDES)
	mkdir -p bin && $(CXX) $(CXX_FLAGS) $(OBJECTS) $< -o $@

bin/%.o: src/%.cc $(INCLUDES)
	mkdir -p bin && $(CXX) -c $(CXX_FLAGS) $< -o $@

clean:
	rm -rf bin/*
