#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"

// Changes read & write behaviour
// If true, reads from stdin and writes to stdout
// If false, reads from a file and writes to a file
// I've modified the test script to allow this
// Because unix pipes are cool :)
#define UNIX_MODE false
#define FILE_MODE !UNIX_MODE

int max(int l, int r)
{
    return l > r ? l : r;
}

int main(int argc, char **argv) // int argc, char **argv
{
    if (argc < 2 && FILE_MODE)
    {
        printf("No args provided");
        return -1;
    }

    const size_t BUFFER_SIZE = 1024 * 1024 * 32; // 32 MB
    // That's a huge buffer. If you use the whole thing, you're doing something stupid.

    char *fileContentBuffer = calloc(BUFFER_SIZE, sizeof(char));

    if (FILE_MODE)
    {
        const char *targetFileName = argv[1];
        FILE *targetFile = fopen(targetFileName, "r"); // "r" for read
        // Read the entire file into a buffer
        fread(fileContentBuffer, sizeof(char), BUFFER_SIZE, targetFile);
        fclose(targetFile); // I miss RAII :(
    }
    else if (UNIX_MODE)
    {
        // Read from stdin
        fread(fileContentBuffer, sizeof(char), BUFFER_SIZE, stdin);
    }
    else
    {
        printf("How did you compile this? (Bad mode)");
        return -1;
    }

    // FILE *oldStdOut = stdout;

    // if (FILE_MODE)
    // {
    //     const char *targetFileName = argv[1];
    //     // ${targetFileName}.lexer
    //     char *outputFileName = calloc(strlen(targetFileName) + 7, sizeof(char));
    //     strcpy(outputFileName, targetFileName);
    //     strcat(outputFileName, ".lexer");
    //     stdout = fopen(outputFileName, "w");
    // }

    size_t fileLength = strlen(fileContentBuffer);

    if (fileLength == BUFFER_SIZE)
    {
        // Dude what did you try to load
        printf("File too large");
        return -1;
    }

    size_t offset = 0;

    struct lexer_token token = lexer_tokenize(fileContentBuffer, fileLength);

    offset += max(token.tokenLength, 1); // Unknown token length is zero, so we need to increment by at least one

    FILE *desiredOutput = FILE_MODE ? 0 : stdout;

    if (desiredOutput == 0)
    {
        const char *targetFileName = argv[1];
        // ${targetFileName}.lexer
        char *outputFileName = calloc(strlen(targetFileName) + 7, sizeof(char));
        strcpy(outputFileName, targetFileName);
        strcat(outputFileName, ".lexer");
        desiredOutput = fopen(outputFileName, "w");
    }

    while (token.type != TOKEN_TYPE_END_OF_FILE)
    {
        do
        {
            switch (token.type)
            {
            case TOKEN_TYPE_WHITESPACE:
            case TOKEN_TYPE_NEWLINE:
            case TOKEN_TYPE_END_OF_FILE:
                break; // Don't display whitespace, newlines, or EOF
            default:
                printToken(&token, desiredOutput);
            }
        } while (false);
        token = lexer_tokenize(fileContentBuffer + offset, fileLength - offset);
        offset += max(token.tokenLength, 1);
    }

    return 0;
}