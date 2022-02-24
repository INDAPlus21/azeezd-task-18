#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALPHABET "ABCDEFGHIJKLMNOPQRSTUVWXYZÄÅÖ"
#define NUMBERS "0123456789"
#define OFFSET ('a' - 'A')
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

static long chars[256] = {0}; // All characters in the encoding stored in array

// Just strcmp but I wanted to have more control
int matching(const unsigned char *buffer, const unsigned char *starting)
{
    int matching = 1;
    for (; *buffer != '\0' && *starting != '\0'; buffer++, starting++)
        matching &= (*buffer == *starting);

    return matching;
}

// Takes a FILE pointer and the position currently in. Returns the current charater pointed at and increments the position counter
int fReadChar(FILE *fp, long *position)
{
    ++*position;
    return fgetc(fp);
}

void init()
{
    // Sets all alphabetical characters to their lower counterpart, otherwise 0
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
    // Set all numerical characters to their values, otherwise 0.

    unsigned char *s = NULL;
    for (size_t i = 0; i < 256; i++)
        chars[i] = 0;
    
    for (s = (unsigned char *)NUMBERS; *s; s++)
        chars[*s] = *s;
}

// Takes the word, searches and returns where the starting position for the index file is from the magic file
long getMagicPos(const char *word)
{
    FILE *magic_fp = NULL;
    unsigned char *buf_ptr = NULL;
    int curr_char;
    char buffer[64] = {'\0'}; // Buffer to work with the words
    char starting[4] = {'\0'}; // Buffer to store the first 3 characters
    long position = 0; // Position in file (throwaway)

    // Get the searched words starting characters
    starting[0] = word[0];
    if (word[1] != '\0')
    {
        starting[1] = word[1];
        starting[2] = word[2];
    }

    magic_fp = fopen("magic", "r");

    do
    {
        // Skip non alphabetical characters
        for (curr_char = fReadChar(magic_fp, &position); curr_char != EOF && chars[curr_char] == 0; curr_char = fReadChar(magic_fp, &position));

        // Get starting character stored
        buf_ptr = buffer;
        *buf_ptr++ = chars[curr_char];
        for (curr_char = fReadChar(magic_fp, &position); curr_char != EOF && chars[curr_char]; curr_char = fReadChar(magic_fp, &position))
            *buf_ptr++ = chars[curr_char];
        *buf_ptr = '\0';

        // Examine if the same start with the searched word
        if (matching(buffer, starting))
        {
            buf_ptr = buffer;
            for (curr_char = fReadChar(magic_fp, &position); curr_char != EOF && curr_char != '\n'; curr_char = fReadChar(magic_fp, &position))
                *buf_ptr++ = curr_char;
            *buf_ptr = '\0';

            fclose(magic_fp);
            return atol(buffer); // Returns the first occurance with the same starting characters in the index file
        }

    } while (curr_char != EOF);

    fclose(magic_fp);
    return -1;
}

long getIndexPos(long pos, const char* word)
{
    FILE *index_fp = NULL;
    unsigned char *buf_ptr = NULL;
    int curr_char;
    long position = pos;
    long word_pos; // Position of the searched word in the index file 
    unsigned char buffer[128];
    index_fp = fopen("index", "r");

    fseek(index_fp, pos, SEEK_SET); // Go to given position from magic file

    do // This is linear search I know :( but it's fast enough... I hope 
    {
        // Skip non-alphabetical characters
        for (curr_char = fReadChar(index_fp, &position); curr_char != EOF && chars[curr_char] == 0; curr_char = fReadChar(index_fp, &position));

        word_pos = position - 1;

        // Get word encountered
        buf_ptr = buffer;
        *buf_ptr++ = chars[curr_char];
        for (curr_char = fReadChar(index_fp, &position); curr_char != EOF && chars[curr_char]; curr_char = fReadChar(index_fp, &position))
            *buf_ptr++ = chars[curr_char];
        *buf_ptr = '\0';

        // If it is the searched word, return its position
        if (strcmp(buffer, word) == 0)
        {
            fclose(index_fp);
            return word_pos;
        }

    } while (curr_char != EOF);

    fclose(index_fp);
    return -1;
}

void writeFromBigFile(long index_pos, const char* word)
{
    init_numbers(); // Switch from alphabetical mode to numerical mode
    FILE* big = NULL;
    unsigned char *buf_ptr = NULL;
    int curr_char;

    long amount = 0; // Amount of occurances in the file
    long positions[25] = {0}; // Stores only 25 occurances
    long position = 0; // Throwaway for read from file
    long *p_ptr = positions; // Pointer to positions array. Used to fill it.
    unsigned char buffer[128];

    big = fopen("index", "r");
    fseek(big, index_pos, SEEK_SET); // Seek to word according to index


    do
    {

        // Skip non-numerical values
        for (curr_char = fReadChar(big, &position); curr_char != EOF && chars[curr_char] == 0 && curr_char != '\n'; curr_char = fReadChar(big, &position));


        buf_ptr = buffer;

        if (amount < 25) // Store first 25 occurances
        {
            // Get index as str
            *buf_ptr++ = chars[curr_char];
            for (curr_char = fReadChar(big, &position); curr_char != EOF && chars[curr_char] && curr_char != '\n'; curr_char = fReadChar(big, &position))
                *buf_ptr++ = chars[curr_char];
            *buf_ptr = '\0';

            // Store in positions array as long
            *p_ptr++ = atol(buffer);

        } else {
            // Just read through all other indices for counting
            for (curr_char = fReadChar(big, &position); curr_char != EOF && chars[curr_char] && curr_char != '\n'; curr_char = fReadChar(big, &position));
        }

        amount++;
        if (curr_char == '\n') break;

    } while (curr_char != EOF);

    fclose(big);

    printf("Det finns %ld förekomster av ordet %s\n", amount, word);

    // Go through Korpus and print all occurrances and the 30 characters around them
    big = fopen("_korpus", "r");
    for (size_t i = 0; i < MIN(25, amount); i++)
    {
        fseek(big, positions[i] - 30 + strlen(word) / 2, SEEK_SET);
        for (size_t j = 0; j < 60; j++) 
        {
            curr_char = fReadChar(big, &position);
            if (curr_char != '\n')
                putchar(curr_char);
            else putchar(' ');
        }
        putchar('\n');
    }
}

int main(int argc, unsigned char *argv[])
{
    init();
    // Convert word to lowercase
    unsigned char *word = argv[1];
    for (unsigned char *_char = word; *_char != '\0'; _char++)
        *_char = chars[*_char];

    long m_idx = getMagicPos(word);
    if (m_idx == -1) // Word not found
    {
        printf("Ordet %s finns inte i filen.", word);
        return 0;
    }

    long i_idx = getIndexPos(m_idx, word);
    if (i_idx == -1) // Word not found
    {
        printf("Ordet %s finns inte i filen.", word);
        return 0;
    }

    writeFromBigFile(i_idx, word);

    return 0;
}