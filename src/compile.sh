#! env bash

# Wrapper script to compile IR to object code
# install into bin

# @arg $1 : path/to/code.dcf
# @arg $2 opt : path/to/executable, defaults to ./a.out

if [[ "$#" -lt 1 ]] ; then
	echo "Usage: bin/compile <path/to/code.dcf> [<path/to/executable>] [clang-opts]"
	exit
fi

code=$1

if [[ "$#" -lt 2 ]] ; then
	out=a.out
else
	out=$2
fi

shift
shift

./bin/decaf $code --output=bin/.temp.ll
clang++ $@ -Wno-override-module build/builtins.o bin/.temp.ll -o $out
