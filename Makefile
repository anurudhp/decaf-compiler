OPTS=-Wno-implicit-function-declaration
CC=gcc

all:
	@echo "Usage:"
	@echo "- scanner"
	@echo "- parser"

scanner: src/scanner.l
	flex -o build/lex.yy.c src/scanner.l
	
parser: src/parser.y scanner
	bison -o build/parser.tab.c -d src/parser.y
	$(CC) $(OPTS) -o bin/parser build/parser.tab.c build/lex.yy.c -ll

clean:
	@rm -rf build/* bin/*

.PHONY: scanner parser clean