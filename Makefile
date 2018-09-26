OPTS=-Wno-implicit-function-declaration
CC=gcc
BISON_OPTS=

all:
	@echo "Usage:"
	@echo "- scanner"
	@echo "- parser"

build/lex.yy.c: src/scanner.l
	flex -o build/lex.yy.c src/scanner.l
	
build/parser.tab.c: src/parser.y build/lex.yy.c
	bison -o build/parser.tab.c -d src/parser.y $(BISON_OPTS)

bin/parser: build/parser.tab.c
	$(CC) $(OPTS) -o bin/parser build/parser.tab.c build/lex.yy.c -ll

parser: bin/parser
scanner: build/lex.yy.c

clean:
	@rm -rf build/* bin/*

.PHONY: clean parser scanner