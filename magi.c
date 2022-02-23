#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 512
#define CMP_START(buf, start) (start[0] == buf[0] && start[1] == buf[1] && start[2] == buf[2])
#define ALPHABET "ABCDEFGHIJKLMNOPQRSTUVWXYZÄÅÖ"
#define OFFSET ('a' - 'A')

static long pos = 0;
static unsigned char u2l[256] = {0};

static void init()
{
    unsigned char *s = NULL;
    unsigned char ch;
    for (unsigned char *s = (unsigned char *)ALPHABET; *s; s++)
    {
        ch = *s + OFFSET;
        u2l[*s] = u2l[ch] = ch;
    }
}

static int readChar()
{
    pos++;
    return getchar();
}

int main(int argc, char const *argv[])
{
    init();
    unsigned char buffer[BUFFER_SIZE] = {"\0"};
    unsigned char starting[4] = {'\0'};
    unsigned char *s = NULL;
    int c;
    long start;
    do
    {
        for (c = readChar(); c != EOF && u2l[c] == 0; c = readChar());
        if (c == EOF)
            break;

        start = pos - 1;
        s = buffer;

        *s++ = u2l[c];
        for (c = readChar(); c != EOF && u2l[c]; c = readChar())
            *s++ = u2l[c];
        *s = '\0';
        if (c == EOF)
            break;

        if (!CMP_START(buffer, starting))
        {
            starting[0] = buffer[0];
            if (buffer[1] == ' ')
                starting[1] = starting[2] = ' ';
            else
            {
                starting[1] = buffer[1];
                starting[2] = buffer[2];
            }

            printf("%s %ld\n", starting, start);
        }

    } while (c != EOF);

    return 0;
}