gcc _tokenizer.c
a < _korpus | sort /l C > token
zig run indexer.zig < token > index
gcc magi.c
a < index > magic