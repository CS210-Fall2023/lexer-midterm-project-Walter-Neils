#include "lexer.h"

#include <stdio.h>

struct lexer lexer_create(char *text)
{
    struct lexer lexer;
    struct lexer_state state;

    state.text = text;
    state.currentPosition = 0;
    state.length = strlen(text);
    lexer.state = state;
    return lexer;
}

typedef int (*lexer_token_caplen_checker)(const char *text, int length);
typedef struct lexer_token *(*lexer_token_parser)(const char *text, int length);

struct complete_lexer_token_parser_bundle
{
    lexer_token_caplen_checker lengthChecker;
    lexer_token_parser parser;
};

struct lexer_bundle_array
{
    struct complete_lexer_token_parser_bundle **bundles;
    int length;
};

struct lexer_token *lexer_token_parse(const char *text, int length)
{
    // TODO: Move the lexer_bundle_array init logic into the lexer_state object because this doesn't need to happen
    // every parse

    struct lexer_bundle_array token_parser_bundles;
    token_parser_bundles.bundles = malloc(1); // TODO: Fix the USE_PARSER macro
    token_parser_bundles.length = 0;
#define USE_PARSER(name)                                                                                               \
    {                                                                                                                  \
        struct complete_lexer_token_parser_bundle *bundle = malloc(sizeof(struct complete_lexer_token_parser_bundle)); \
        bundle->lengthChecker = LEXER_TOKEN_PARSE_HANDLER_CAPLEN_NAME(name);                                           \
        bundle->parser = LEXER_TOKEN_PARSE_HANDLER_PARSE_NAME(name);                                                   \
        token_parser_bundles.bundles =                                                                                 \
            realloc(token_parser_bundles.bundles,                                                                      \
                    token_parser_bundles.length * sizeof(struct complete_lexer_token_parser_bundle) +                  \
                        sizeof(struct complete_lexer_token_parser_bundle));                                            \
        token_parser_bundles.bundles[token_parser_bundles.length] = bundle;                                            \
        token_parser_bundles.length++;                                                                                 \
    }

    // Setup the parsers here
    USE_PARSER(number);
    USE_PARSER(string);
    USE_PARSER(whitespace);
    USE_PARSER(newline);

    struct complete_lexer_token_parser_bundle *currentBestMatch = 0;
    int bestCaptureLength = 0;

    // Determine which parser bundle can capture the largest portion of text
    for (int i = 0; i < token_parser_bundles.length; i++)
    {
        struct complete_lexer_token_parser_bundle *targetBundle = *(token_parser_bundles.bundles + i);
        int capturableLength = targetBundle->lengthChecker(text, length);
        if (capturableLength > bestCaptureLength)
        {
            bestCaptureLength = capturableLength;
            currentBestMatch = targetBundle;
        }
    }

    if (currentBestMatch == 0)
    {
        // No recognized tokens
        struct lexer_token *result = malloc(sizeof(struct lexer_token));
        lexer_token_initialize(result);
        result->text = text;
        // Walk forward until a token parser can capture the text
        for (int i = 0; i < length; i++)
        {
            for (int j = 0; j < token_parser_bundles.length; j++)
            {
                struct complete_lexer_token_parser_bundle *targetBundle = *(token_parser_bundles.bundles + j);
                int capturableLength = targetBundle->lengthChecker(text + i, length - i);
                if (capturableLength > 0)
                {
                    result->length = i;
                    break;
                }
            }
        }
        result->type = LEXER_TOKEN_TYPE_UNKNOWN;
        return result;
    }
    else
    {
        return currentBestMatch->parser(text, bestCaptureLength);
    }

#undef USE_PARSER
}

void lexer_state_add_child(struct lexer_state *lexer_state, struct lexer_token *child)
{
    if (lexer_state->children == 0)
    {
        lexer_state->children = malloc(sizeof(struct lexer_token *));
    }
    else if (lexer_state->childCount > 0)
    {
        lexer_state->children =
            realloc(lexer_state->children, sizeof(struct lexer_token *) * (lexer_state->childCount + 1));
    }

    lexer_state->children[lexer_state->childCount] = child;
    lexer_state->childCount++;
}

void lexer_token_initialize(struct lexer_token *token)
{
    token->childCount = 0;
    token->children = 0;
    token->length = 0;
    token->text = 0;
}

void lexer_token_child_add(struct lexer_token *token, struct lexer_token *child)
{
    if (token->children == 0)
    {
        token->children = malloc(sizeof(struct lexer_token *));
    }
    else if (token->childCount > 0)
    {
        token->children = realloc(token->children, sizeof(struct lexer_token *) * (token->childCount + 1));
    }

    token->children[token->childCount] = child;
    token->childCount++;
}

char *lexer_token_get_text(struct lexer_token *token)
{
    char *result = malloc(token->length + 1);
    memcpy(result, token->text, token->length);
    result[token->length] = '\0';
    return result;
}

LEXER_TOKEN_PARSE_HANDLER_CAPLEN_DECL(number)
{
    int capturableLength = 0;

    for (int i = 0; i < length; i++)
    {
        // 48 - 57 is the valid range of numbers
        char value = *(text + i);
        if (value > 57 || value < 48)
        {
            break; // Not a valid number, need to bail
        }
        capturableLength++;
    }

    return capturableLength;
}

LEXER_TOKEN_PARSE_HANDLER_PARSE_DECL(number)
{
    struct lexer_token *result = (struct lexer_token *)malloc(sizeof(struct lexer_token));

    result->childCount = 0;
    result->children = 0;
    result->length = 0;
    result->text = text;

    result->type = LEXER_TOKEN_TYPE_NUMBER;

    for (int i = 0; i < length; i++)
    {
        // 48 - 57 is the valid range of numbers
        char value = *(text + i);
        if (value > 57 || value < 48)
        {
            break; // Not a valid number, need to bail
        }
        result->length++;
    }

    return result;
}

LEXER_TOKEN_PARSE_HANDLER_CAPLEN_DECL(string)
{
    int capturableLength = 0;

    if (length > 0 && *text == '"')
    {
        capturableLength++;
        for (int i = 1; i < length; i++)
        {
            char value = *(text + i);
            if (value == '"')
            {
                capturableLength++;
                break;
            }
            capturableLength++;
        }
    }

    return capturableLength;
}

LEXER_TOKEN_PARSE_HANDLER_PARSE_DECL(string)
{
    struct lexer_token *result = (struct lexer_token *)malloc(sizeof(struct lexer_token));

    result->childCount = 0;
    result->children = 0;
    result->length = 0;
    result->text = text;

    result->type = LEXER_TOKEN_TYPE_STRING;

    if (length > 0 && *text == '"')
    {
        result->length++;
        for (int i = 1; i < length; i++)
        {
            char value = *(text + i);
            if (value == '"')
            {
                result->length++;
                break;
            }
            result->length++;
        }
    }

    return result;
}

LEXER_TOKEN_PARSE_HANDLER_CAPLEN_DECL(whitespace)
{
    int capturableLength = 0;

    for (int i = 0; i < length; i++)
    {
        char value = *(text + i);
        if (value != ' ' && value != '\t')
        {
            break;
        }
        capturableLength++;
    }

    return capturableLength;
}

LEXER_TOKEN_PARSE_HANDLER_PARSE_DECL(whitespace)
{
    struct lexer_token *result = (struct lexer_token *)malloc(sizeof(struct lexer_token));

    result->childCount = 0;
    result->children = 0;
    result->length = 0;
    result->text = text;

    result->type = LEXER_TOKEN_TYPE_WHITESPACE;

    for (int i = 0; i < length; i++)
    {
        char value = *(text + i);
        if (value != ' ' && value != '\t')
        {
            break;
        }
        result->length++;
    }

    return result;
}

LEXER_TOKEN_PARSE_HANDLER_CAPLEN_DECL(newline)
{
    int capturableLength = 0;

    for (int i = 0; i < length; i++)
    {
        char value = *(text + i);
        if (value != '\n')
        {
            break;
        }
        capturableLength++;
    }

    return capturableLength;
}

LEXER_TOKEN_PARSE_HANDLER_PARSE_DECL(newline)
{
    struct lexer_token *result = (struct lexer_token *)malloc(sizeof(struct lexer_token));

    result->childCount = 0;
    result->children = 0;
    result->length = 0;
    result->text = text;

    result->type = LEXER_TOKEN_TYPE_NEWLINE;

    for (int i = 0; i < length; i++)
    {
        char value = *(text + i);
        if (value != '\n')
        {
            break;
        }
        result->length++;
    }

    return result;
}