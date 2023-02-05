CXXFLAGS = -Wall -pedantic -std=c++11 -g -Wno-sign-compare
LINK.o = $(LINK.cc)

PROGRAMS = main

all: clean main test

clean: 
	@rm -f *.o

main: main.o generator.o
test: test.o generator.o