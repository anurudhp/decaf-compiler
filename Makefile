CC_OPTS=-std=c++14 -Wall -ll -g
CC=g++
FLEX_OPTS=
BISON_OPTS=

HEADERS=ast visitor
SRCS=literals operators variables statements blocks methods program

OBJS=$(patsubst %,build/%.o,$(SRCS))

all: parser

src/lex.yy.cc: src/scanner.ll
	flex $(FLEX_OPTS) -o $@ $<

src/parser.tab.cc: src/parser.yy src/lex.yy.cc
	bison -o $@ -d $< $(BISON_OPTS)

build/%.o: src/astnodes/%.cc src/astnodes/%.hh
	$(CC) -c -o $@ $< $(CC_OPTS)

build/%.o: src/visitors/%.cc scr/visitors/%.hh
	$(CC) -c -o $@ $< $(CC_OPTS)


build/lex.o: src/lex.yy.cc src/parser.tab.cc
	$(CC) -c -o $@ $< $(CC_OPTS)

build/parser.o: src/parser.tab.cc
	$(CC) -c -o $@ $< $(CC_OPTS)

bin/decaf: build/lex.o build/parser.o $(OBJS)
	$(CC) -o $@ $^ $(CC_OPTS)

parser: bin/decaf

test: parser
	@for i in `ls test-programs`; do  			 			\
		echo program: $$i ;									\
		./bin/decaf <test-programs/$$i >/dev/null;			\
	done;

clean:
	@rm -rf bin/* build/*
	@rm -f src/lex.yy.cc src/parser.tab.* src/stack.hh src/location.hh src/position.hh src/parser.output 

.PHONY: clean test parser
