#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ALPHABET "ABCDEFGHIJKLMNOPQRSTUVWXYZÄÅÖ"
#define NUMBERS "0123456789"
#define OFFSET ('a' - 'A')
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

static long chars[256] = {0};

int matching(const unsigned char *buffer, const unsigned char *starting)
{
    int matching = 1;
    for (; *buffer != '\0' && *starting != '\0'; buffer++, starting++)
        matching &= (*buffer == *starting);

    return matching;
}

int fReadChar(FILE *fp, long *position)
{
    ++*position;
    return fgetc(fp);
}

void init()
{
    unsigned char *s = NULL;
    unsigned char ch;
    for (s = (unsigned char *)ALPHABET; *s; s++)
    {
        ch = *s + OFFSET;
        chars[ch] = chars[*s] = ch;
    }
}

void init_numbers()
{
    unsigned char *s = NULL;
    for (size_t i = 0; i < 256; i++)
        chars[i] = 0;
    
    for (s = (unsigned char *)NUMBERS; *s; s++)
        chars[*s] = *s;
}

long getMagicPos(const char *word)
{
    FILE *magic_fp = NULL;
    unsigned char *s = NULL;
    int ch;
    char buffer[64] = {'\0'};
    char starting[4] = {'\0'};
    long position = 0;

    starting[0] = word[0];
    if (word[1] != '\0')
    {
        starting[1] = word[1];
        starting[2] = word[2];
    }

    magic_fp = fopen("magic", "r");

    do
    {
        for (ch = fReadChar(magic_fp, &position); ch != EOF && chars[ch] == 0; ch = fReadChar(magic_fp, &position))
            ;
        s = buffer;
        *s++ = chars[ch];
        for (ch = fReadChar(magic_fp, &position); ch != EOF && chars[ch]; ch = fReadChar(magic_fp, &position))
            *s++ = chars[ch];

        if (matching(buffer, starting))
        {
            s = buffer;
            for (ch = fReadChar(magic_fp, &position); ch != EOF && ch != '\n'; ch = fReadChar(magic_fp, &position))
                *s++ = ch;
            *s = '\0';

            fclose(magic_fp);
            return atol(buffer);
        }

    } while (ch != EOF);

    fclose(magic_fp);
    return -1;
}

long getIndexPos(long pos, const char* word)
{
    FILE *index_fp = NULL;
    unsigned char *s = NULL;
    int ch;
    long position = pos;
    long word_pos;
    unsigned char buffer[128];
    index_fp = fopen("index", "r");

    fseek(index_fp, pos, SEEK_SET);

    do
    {
        for (ch = fReadChar(index_fp, &position); ch != EOF && chars[ch] == 0; ch = fReadChar(index_fp, &position));

        word_pos = position - 1;

        s = buffer;
        *s++ = chars[ch];
        for (ch = fReadChar(index_fp, &position); ch != EOF && chars[ch]; ch = fReadChar(index_fp, &position))
            *s++ = chars[ch];
        *s = '\0';

        if (strcmp(buffer, word) == 0)
        {
            fclose(index_fp);
            return word_pos;
        }

    } while (ch != EOF);

    fclose(index_fp);
    return -1;
}

void writeFromBigFile(long index_pos, const char* word)
{
    init_numbers();
    FILE* big = NULL;
    unsigned char *s = NULL;
    int ch;
    long amount = 0;
    long positions[25] = {0};
    long position = 0;
    long *p_ptr = positions;
    unsigned char buffer[128];
    big = fopen("index", "r");
    fseek(big, index_pos, SEEK_SET);
    do
    {
        for (ch = fReadChar(big, &position); ch != EOF && chars[ch] == 0 && ch != '\n'; ch = fReadChar(big, &position));
        s = buffer;
        if (amount < 25) {
            *s++ = chars[ch];
            for (ch = fReadChar(big, &position); ch != EOF && chars[ch] && ch != '\n'; ch = fReadChar(big, &position))
                *s++ = chars[ch];
            *s = '\0';
            *p_ptr++ = atol(buffer);

        } else {
            for (ch = fReadChar(big, &position); ch != EOF && chars[ch] && ch != '\n'; ch = fReadChar(big, &position));
        }

        amount++;
        if (ch == '\n') break;

    } while (ch != EOF);

    fclose(big);

    printf("Det finns %ld förekomster av ordet %s\n", amount, word);

    big = fopen("_korpus", "r");

    for (size_t i = 0; i < MIN(25, amount); i++)
    {
        fseek(big, positions[i] - 30, SEEK_SET);
        for (size_t j = 0; j < 60; j++) 
        {
            ch = fReadChar(big, &position);
            if (ch != '\n')
                putchar(ch);
            else putchar(' ');
        }
        putchar('\n');
    }
}

int main(int argc, char const *argv[])
{
    init();
    FILE *big_fp = NULL;
    const char *word = argv[1];
    long idx = getIndexPos(getMagicPos(word), word);
    writeFromBigFile(idx, word);

    return 0;
}