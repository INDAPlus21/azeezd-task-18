@ECHO OFF
echo Constructing...
echo Tokenizing File...
gcc _tokenizer.c
a < _korpus | sort /l C > token

echo Generating Index File...
zig run indexer.zig < token > index

echo Generating Magic File...
gcc magi.c
a < index > magic

echo Cleaning Up!
del /f a.exe
gcc concordan.c -o concordance
chcp 28591
echo Done!