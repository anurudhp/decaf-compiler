CXX=g++
CXX_OPTS=-g
LLVM_OPTS=`llvm-config --cxxflags --ldflags` -fexceptions
LLVM_LINK_OPTS=`llvm-config --libs --libfiles --system-libs`
FLEX_OPTS=
BISON_OPTS=-v

HEADERS=ast visitor
SRCS=ast literals operators variables statements blocks methods program \
	treegen semantic_analyzer codegen \
	driver lex parser

OBJS=$(patsubst %,build/%.o,$(SRCS))

all: parser

src/lex.yy.cc: src/scanner.ll
	flex $(FLEX_OPTS) -o $@ $<

src/parser.tab.cc: src/parser.yy src/lex.yy.cc
	bison -o $@ -d $< $(BISON_OPTS)

build/ast.o: src/ast/ast.cc src/ast/ast.hh src/parser.tab.cc
	$(CXX) -c -o $@ $< $(CXX_OPTS) $(LLVM_OPTS)

build/%.o: src/ast/%.cc src/ast/%.hh
	$(CXX) -c -o $@ $< $(CXX_OPTS) $(LLVM_OPTS)

build/%.o: src/visitors/%.cc src/visitors/%.hh
	$(CXX) -c -o $@ $< $(CXX_OPTS) $(LLVM_OPTS)

build/driver.o: src/driver.cc src/driver.hh src/parser.tab.cc
	$(CXX) -c -o $@ $< $(CXX_OPTS) $(LLVM_OPTS)

build/lex.o: src/lex.yy.cc src/parser.tab.cc
	$(CXX) -c -o $@ $< $(CXX_OPTS) $(LLVM_OPTS)

build/parser.o: src/parser.tab.cc
	$(CXX) -c -o $@ $< $(CXX_OPTS) $(LLVM_OPTS)

build/builtins.o: src/builtins/*.cc
	$(CXX) -c -o $@ src/builtins/*.cc $(CXX_OPTS) $(LLVM_OPTS)

bin/decaf: $(OBJS) build/builtins.o
	$(CXX) -o $@ $^ $(LLVM_LINK_OPTS) $(CXX_OPTS) $(LLVM_OPTS)

parser: bin/decaf
	cp src/compile.sh bin/compile && chmod +x bin/compile

test: parser
	@for i in `ls test-programs`; do  			 			\
		echo program: $$i ;									\
		./bin/decaf test-programs/$$i ;			\
	done;

clean:
	@cp bin/readme.md bin/.readme.md
	@cp build/readme.md build/.readme.md
	@rm -rf bin/* build/*
	@mv bin/.readme.md bin/readme.md
	@mv build/.readme.md build/readme.md
	@rm -f src/lex.yy.cc src/parser.tab.* src/stack.hh src/location.hh src/position.hh src/parser.output 

.PHONY: clean test parser
