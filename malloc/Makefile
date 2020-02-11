CC=gcc
CFLAGS=-O0 -g -W -Wall -Itest -Iutility
SHARED=-shared -fPIC
DEPS= utility/debug.h
DBG=utility/debug.c
BUILD=build
DBGOBJ=$(DBG:.c=.o)

SIMPLE-TESTS=$(wildcard test/simple/*.c)
SIMPLE-TESTFILES=$(SIMPLE-TESTS:.c=)
SIMPLE-TESTRUN=$(SIMPLE-TESTS:.c=.run)

FULL-TESTS=$(wildcard test/full/*.c)
FULL-TESTFILES=$(FULL-TESTS:.c=)
FULL-TESTRUN=$(FULL-TESTS:.c=.run)

all: $(SIMPLE-TESTFILES) $(FULL-TESTFILES)

.PHONY: test-simple
.PHONY: test-full
.PHONY: clean

%.so: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

%.run: %
	LD_PRELOAD=$(CURDIR)/$(BUILD)/malloc.so ./$<

$(SIMPLE-TESTFILES): %: %.o $(DBGOBJ)
	 gcc -o $@ $^

$(FULL-TESTFILES): %: %.o $(DBGOBJ)
	 gcc -o $@ $^

malloc-simple:
	$(CC) $(CFLAGS) $(SHARED) malloc-simple.c $(DBG) -o $(BUILD)/malloc-simple.so

malloc:
	$(CC) $(CFLAGS) $(SHARED) malloc.c $(DBG) -o $(BUILD)/malloc.so

test-simple: $(SIMPLE-TESTRUN)

test-full: $(SIMPLE-TESTRUN) $(FULL-TESTRUN)

test-strcmp:
	$(CC) $(CFLAGS) test/strcmp_test.c $(DBG) -o $(BUILD)/strcmp_test

clean:
	$(RM) $(BUILD)/*
