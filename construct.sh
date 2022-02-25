#!/bin/bash
printf "Constructing...\n"
printf "Tokenizing File...\n"
gcc _tokenizer.c
export LC_COLLATE=C 
./a.out < _korpus | sort -S 50% --parallel=4 > token

printf "Generating Index File...\n"
zig run indexer.zig < token > index

printf "Generating Magic File...\n"
gcc magi.c
./a.out < index > magic

printf "Cleaning up!\n"
rm ./a.out
gcc concordan.c -o concordance

printf "Done!\n"