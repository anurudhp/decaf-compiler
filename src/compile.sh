#! env bash

# Wrapper script to compile IR to binary
# install script to `bin`

# @arg $1 : path/to/code.dcf
# @arg $2 opt : path/to/executable, defaults to ./a.out

if [[ "$#" -lt 1 ]] ; then
	echo "Usage: bin/compile <path/to/code.dcf> [clang-opts]"
	exit
fi

code=$1
./bin/decaf $code --output=bin/.temp.ll

shift
clang++ $@ -Wno-override-module build/builtins.o bin/.temp.ll
