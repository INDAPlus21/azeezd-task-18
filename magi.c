#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 512
#define CMP_START(buf, start) (start[0] == buf[0] && start[1] == buf[1] && start[2] == buf[2])
#define ALPHABET "ABCDEFGHIJKLMNOPQRSTUVWXYZÄÅÖ"
#define OFFSET ('a' - 'A')

static long pos = 0; // Position in file
static unsigned char u2l[256] = {0}; // Upper to low. Storing all possible 8-bit characters

static void init()
{
    // Converting all upper to lower and setting all non alphabetical characters to 0 (in order to ignore them)
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
    unsigned char starting[4] = {'\0'}; // The 3 letter start + 1 for null terminator
    unsigned char *buf_ptr = NULL;
    int curr_char;
    long start; // Where the word starts
    do
    {
        // Skip non alphabetical characters
        for (curr_char = readChar(); curr_char != EOF && u2l[curr_char] == 0; curr_char = readChar());
        if (curr_char == EOF)
            break;

        start = pos - 1; // Save position of the word to print later on

        // Get the word currently pointed at. Stop at non alphabetical character
        buf_ptr = buffer;
        *buf_ptr++ = u2l[curr_char];
        for (curr_char = readChar(); curr_char != EOF && u2l[curr_char]; curr_char = readChar())
            *buf_ptr++ = u2l[curr_char];
        *buf_ptr = '\0';

        if (curr_char == EOF)
            break;

        // Compare first three letters (or less if shorter)
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

    } while (curr_char != EOF);

    return 0;
}