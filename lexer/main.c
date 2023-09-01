#include <stdio.h>
#include "lexer.h"

void fmt_println(char *text)
{
    printf("%s\n", text);
}

int main() // int argc, char **argv
{
    const char *test = "\"testing\"246456\n\n\n   \"x\" 21 21";
    for (size_t i = 0; i < strlen(test);)
    {
        struct lexer_token *result = lexer_token_parse(test + i, strlen(test) - i);

        i += result->length;

        // Print the type of token
        switch (result->type)
        {
        case LEXER_TOKEN_TYPE_NUMBER:
            fmt_println("Number");
            break;
        case LEXER_TOKEN_TYPE_STRING:
            fmt_println("String");
            break;
        case LEXER_TOKEN_TYPE_IDENTIFIER:
            fmt_println("Identifier");
            break;
        case LEXER_TOKEN_TYPE_OPERATOR:
            fmt_println("Operator");
            break;
        case LEXER_TOKEN_TYPE_KEYWORD:
            fmt_println("Keyword");
            break;
        case LEXER_TOKEN_TYPE_COMMENT:
            fmt_println("Comment");
            break;
        case LEXER_TOKEN_TYPE_NEWLINE:
            fmt_println("Newline");
            break;
        case LEXER_TOKEN_TYPE_WHITESPACE:
            fmt_println("Whitespace");
            break;
        default:
            fmt_println("Unknown");
            break;
        }

        char *tokenText = lexer_token_get_text(result);
        printf("Text: <<<%s>>>\n", tokenText);
        free(tokenText);
    }
}