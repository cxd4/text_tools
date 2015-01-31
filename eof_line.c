#include <stdio.h>

extern int is_printable(int character);
extern int is_ASCII(char * path);

int main(int argc, char ** argv)
{
    FILE * stream;
    char * path;
    int character, last_character;
    int unsupported_binary_file;

    path = (argc < 2) ? (char *)"in" : argv[1];
    stream = fopen(path, "r+");
    if (stream == NULL)
    {
        fputs("Failed to access file for streaming.\n", stderr);
        getchar();
        return -1;
    }

    unsupported_binary_file = !is_ASCII(path);
    if (unsupported_binary_file)
        return -2;

/*
 * Carefully seek to the end of the text stream, while analyzing the nature
 * of the file's contents to see if it is worth treating as text data.
 */
    character = fgetc(stream);
    last_character = '\0';
    while (character != EOF)
    {
        if (character < 0)
        {
            fprintf(stderr, "Unknown negative return value:  %i\n", character);
            getchar();
            return -3;
        }
        last_character = character;
        character = fgetc(stream);
    }

/*
 * If the last character of the text file is already a line break ('\n'),
 * then there is no work for this program to do.  Otherwise, append it.
 */
    if (last_character == '\n')
        puts("Text file was already terminated by a newline.");
    else
        fputc('\n', stream);

    while (fclose(stream) != 0);
    return 0;
}

int is_printable(int character)
{
    if (character > 127)
        return 0;

#ifndef TOLERATE_ALL_ASCII
    if (character == 127)
        return 0;

/*
 * The function name "is_printable" is a hacky misnomer currently.
 *
 * I didn't write this program to ignore ALL files without only printables.
 * I just wrote it to process C code files, which have some things like...
 */
    if (character < ' ')
    {
        if (character == 10 || character == 13) /* Unix/Windows newlines */
            return 1;
        if (character == 9) /* tab character, for code whitespace indents */
            return 1; /* (tabs suck btw :)) */
        return 0;
    }
#endif
    return 1;
}

/*
 * Returns false (zero) if the file as a binary stream has non-ASCII bytes.
 * Returns negative one if the specified file path could not be opened.
 */
int is_ASCII(char * path)
{
    FILE * stream;
    int character;

    stream = fopen(path, "rb");
    if (stream == NULL)
        return -1;

    character = fgetc(stream);
    while (character >= 0)
    {
        const int acceptable = is_printable(character);

        if (acceptable == 0)
        {
            fclose(stream);
            return 0; /* one or more invalid ASCII characters */
        }
        character = fgetc(stream);
    }
    fclose(stream);

    if (character == EOF)
        return 1; /* true:  All chars from start to end are valid ASCII. */
    printf("Undocumented fgetc return value:  %i.\n", character);
    return 0; /* ??? other negative return values of fgetc? */
}
