#!/bin/bash
printf "Constructing...\n"
printf "Tokenizing File...\n"
export LC_COLLATE=C 
gcc _tokenizer.c
./a.out < _korpus | LC_ALL=C sort -S 50% --parallel=4 > token

printf "Generating Index File...\n"
zig run LINUX_indexer.zig < token > index

printf "Generating Magic File...\n"
gcc magi.c
./a.out < index > magic

printf "Cleaning up!\n"
rm ./a.out
gcc concordan.c -o concordance

printf "Done!\n"