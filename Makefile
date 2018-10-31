CC_OPTS=-std=c++14 -Wall -ll
CC=g++
FLEX_OPTS=
BISON_OPTS=

all: parser

src/lex.yy.cc: src/scanner.ll
	flex $(FLEX_OPTS) -o $@ $<

src/parser.tab.cc: src/parser.yy src/lex.yy.cc
	bison -o $@ -d $< $(BISON_OPTS)

bin/decaf: src/parser.tab.cc src/lex.yy.cc
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
