#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"

void printToken(struct lexer_token *token)
{
    if (token->type == TOKEN_TYPE_UNKNOWN)
    {
        printf(".");
        return;
    }
    char *tokenType = "unknown";
    switch (token->type)
    {
    case TOKEN_TYPE_UNKNOWN:
        tokenType = "unknown";
        break;
    case TOKEN_TYPE_IDENTIFIER:
        tokenType = "identifier";
        break;
    case TOKEN_TYPE_NUMBER:
        tokenType = "numeric literal";
        break;
    case TOKEN_TYPE_STRING:
        tokenType = "string";
        break;
    case TOKEN_TYPE_OPERATOR:
        tokenType = "operator";
        break;
    case TOKEN_TYPE_KEYWORD:
        tokenType = "keyword";
        break;
    case TOKEN_TYPE_COMMENT:
        tokenType = "comment";
        break;
    case TOKEN_TYPE_NEWLINE:
        tokenType = "newline";
        break;
    case TOKEN_TYPE_WHITESPACE:
        tokenType = "whitespace";
        break;
    case TOKEN_TYPE_PUNCTUATION:
        tokenType = "punctuation";
        break;
    case TOKEN_TYPE_END_OF_FILE:
        tokenType = "eof";
        break;
    default:
        break;
    }
    // TOKEN_CONTENT (TOKEN_TYPE)
    printf("%.*s (%s)\n", token->tokenLength, token->token, tokenType);
}

int max(int l, int r)
{
    return l > r ? l : r;
}

int main(int argc, char **argv) // int argc, char **argv
{
    if (argc < 2)
    {
        printf("No args provided");
        return -1;
    }

    char *fileContentBuffer = calloc(2048, sizeof(char));

    const char *targetFileName = argv[1];
    FILE *targetFile = fopen(targetFileName, "r");
    // Read the entire file into a buffer
    fread(fileContentBuffer, sizeof(char), 2048, targetFile);
    fclose(targetFile); // I miss RAII

    size_t fileLength = strlen(fileContentBuffer);

    size_t offset = 0;

    struct lexer_token token = lexer_tokenize(fileContentBuffer, fileLength);

    offset += max(token.tokenLength, 1);

    while (token.type != TOKEN_TYPE_END_OF_FILE)
    {
        do
        {
            switch (token.type)
            {
            case TOKEN_TYPE_WHITESPACE:
            case TOKEN_TYPE_NEWLINE:
            case TOKEN_TYPE_END_OF_FILE:
                break;
            default:
                printToken(&token);
            }
        } while (false);
        token = lexer_tokenize(fileContentBuffer + offset, fileLength - offset);
        offset += max(token.tokenLength, 1);
    }
    return 0;
}