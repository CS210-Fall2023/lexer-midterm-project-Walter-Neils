#include "lexer.h"

void printToken(struct lexer_token *token, FILE *outputStream)
{
    char *tokenType = "unknown [invalid token type]";
    switch (token->type)
    {
    case TOKEN_TYPE_UNKNOWN:
        tokenType = "unknown [PURPOSELY UNMATCHED]";
        break;
    case TOKEN_TYPE_IDENTIFIER:
        tokenType = "identifier";
        break;
    case TOKEN_TYPE_NUMBER:
        tokenType = "numeric literal";
        break;
    case TOKEN_TYPE_CHARACTER_LITERAL:
        tokenType = "character literal";
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
    fprintf(outputStream, "%.*s (%s)\n", (int)token->tokenLength, token->token, tokenType);
}

struct lexer_token_parser
{
    struct lexer_token (*parser)(const char *content, size_t contentLength);
};
struct lexer_token lexer_tokenize(const char *content, size_t contentLength)
{
    if (contentLength == 0)
    {
        // Empty string. Return EOF token.
        struct lexer_token token;
        token.token = content;
        token.tokenLength = 0;
        token.type = TOKEN_TYPE_END_OF_FILE;
        return token;
    }

    // Parser functions so we can iterate over them
    struct lexer_token_parser parsers[] = {{commentParser},    {keywordParser},    {stringParser},
                                           {numberParser},     {operatorParser},   {characterLiteralParser},
                                           {identifierParser}, {whitespaceParser}, {newlineParser}};

    // Current best token
    struct lexer_token bestToken = {
        .token = content,
        .tokenLength = 0,
        .type = TOKEN_TYPE_UNKNOWN,
    };
    for (size_t i = 0; i < sizeof(parsers) / sizeof(struct lexer_token_parser); i++)
    {
        struct lexer_token token = parsers[i].parser(content, contentLength);
        if (token.tokenLength > bestToken.tokenLength)
        {
            // Better match than previous token
            bestToken = token;
        }
    }

    return bestToken;
}

LEXER_PARSER_FUNCTION(string)
{
    LEXER_TOKEN_INIT_RESULT(TOKEN_TYPE_STRING)
    if (contentLength < 2)
    {
        // Minimum length is 2: ""
        LEXER_TOKEN_CANNOT_CAPTURE();
    }
    if (content[0] != '"')
    {
        // Yeah that's not the start of a string
        LEXER_TOKEN_CANNOT_CAPTURE();
    }
    size_t i = 1;
    for (; i < contentLength; i++)
    {
        if (content[i] == '"')
        {
            // Yep that'd be the end of the string
            break;
        }
    }
    if (i == contentLength)
    {
        // No closing quote
        LEXER_TOKEN_CANNOT_CAPTURE();
    }
    token.tokenLength = i + 1;
    return token;
}

LEXER_PARSER_FUNCTION(number)
{
    LEXER_TOKEN_INIT_RESULT(TOKEN_TYPE_NUMBER);
    if (contentLength == 0)
    {
        LEXER_TOKEN_CANNOT_CAPTURE();
    }

    size_t i = 0;

    if (content[0] == '.')
    {
        // This conditional is really important because otherwise the lexer tries to parse the righthand side of a range
        // as a number
        LEXER_TOKEN_CANNOT_CAPTURE();
    }

    bool hasUsedDecimalPoint = false;

    while (true)
    {
        const bool passConditions[] = {(content[i] >= '0' && content[i] <= '9'), // 0-9
                                       (content[i] == '.' && !hasUsedDecimalPoint && content[i + 1] != '.'),
                                       (content[i] == '#')};

        bool passed = false;
        for (size_t j = 0; j < sizeof(passConditions) / sizeof(bool); j++)
        {
            if (passConditions[j])
            {
                passed = true;
                break;
            }
        }

        if (!passed)
        {
            break;
        }

        if (content[i] == '.')
        {
            hasUsedDecimalPoint = true;
        }

        i++;
    }

    if (i == 0)
    {
        // Ah yes, the classic zero-length number
        // Yessir, that's definitely a number right there
        // See him? He's hiding in between the bytes
        // In case sarcasm isn't obvious, this means we didn't find a number
        LEXER_TOKEN_CANNOT_CAPTURE();
    }

    token.tokenLength = i;

    return token;
}
LEXER_PARSER_FUNCTION(operator)
{
    // These NEED to be in order of longest to shortest
    // Otherwise the lexer will match the first character of a longer operator
    // And that's not quite what we want
    const char *possibleOperatorTokens[] = {
        ":=", "..", "<<", ">>", "<>", "<=", ">=", "**", "!=", "=>", ".", "<", ">", "(",
        ")",  "+",  "-",  "*",  "/",  "|",  "&",  ";",  ",",  ":",  "[", "]", "="};

    LEXER_TOKEN_INIT_RESULT(TOKEN_TYPE_OPERATOR);
    if (contentLength == 0)
    {
        // That's not an operator, that's an empty string.
        LEXER_TOKEN_CANNOT_CAPTURE();
    }

    for (size_t i = 0; i < sizeof(possibleOperatorTokens) / sizeof(char *); i++)
    {
        size_t tokenLen = strlen(possibleOperatorTokens[i]);
        // Through the power of strncmp, we can accomplish anything
        if (strncmp(content, possibleOperatorTokens[i], tokenLen) == 0)
        {
            // I hate the fact that strncmp returns 0 on success
            // You're not a process, strncmp, you're a function
            // You're supposed to return 1 on success so I can use you in if statements without having to negate you
            token.tokenLength = tokenLen;
            return token;
        }
    }

    LEXER_TOKEN_CANNOT_CAPTURE();
}
LEXER_PARSER_FUNCTION(characterLiteral)
{
    LEXER_TOKEN_INIT_RESULT(TOKEN_TYPE_CHARACTER_LITERAL);
    if (contentLength < 3)
    {
        // Minimum length is 3: 'a'
        LEXER_TOKEN_CANNOT_CAPTURE();
    }
    if (content[0] != '\'')
    {
        // Yeah that's not the start of a character literal
        LEXER_TOKEN_CANNOT_CAPTURE();
    }
    size_t i = 1;
    for (; i < contentLength; i++) // This is literally the string logic. It just works (TM)
    {
        if (content[i] == '\'')
        {
            break;
        }
    }
    if (i == contentLength)
    {
        // No closing quote
        LEXER_TOKEN_CANNOT_CAPTURE();
    }
    token.tokenLength = i + 1;
    return token;
}
LEXER_PARSER_FUNCTION(identifier)
{
    // Must start with a letter
    // Can contain letters, numbers, and underscores
    LEXER_TOKEN_INIT_RESULT(TOKEN_TYPE_IDENTIFIER);
    if (contentLength == 0)
    {
        LEXER_TOKEN_CANNOT_CAPTURE();
    }
    if (!((content[0] >= 'a' && content[0] <= 'z') || (content[0] >= 'A' && content[0] <= 'Z')))
    {
        LEXER_TOKEN_CANNOT_CAPTURE();
    }
    size_t i = 1;
    for (; i < contentLength; i++)
    {
        if (!((content[i] >= 'a' && content[i] <= 'z') || (content[i] >= 'A' && content[i] <= 'Z') ||
              (content[i] >= '0' && content[i] <= '9') || content[i] == '_'))
        {
            break;
        }
    }
    token.tokenLength = i;
    return token;
}
LEXER_PARSER_FUNCTION(keyword)
{
    LEXER_TOKEN_INIT_RESULT(TOKEN_TYPE_KEYWORD);
    if (contentLength == 0)
    {
        LEXER_TOKEN_CANNOT_CAPTURE();
    }

    // Again, these need to be in order of longest to shortest
    // Otherwise the lexer will match the first character of a longer keyword
    // Which is not what we want
    const char *keywords[] = {"character", "interface", "procedure", "accessor", "constant", "function", "positive",
                              "integer",   "mutator",   "natural",   "subtype",  "module",   "return",   "struct",
                              "array",     "begin",     "elsif",     "other",    "range",    "while",    "bool",
                              "case",      "else",      "exit",      "loop",     "null",     "then",     "type",
                              "when",      "and",       "end",       "out",      "if",       "in",       "is",
                              "of",        "or"};
    for (size_t i = 0; i < sizeof(keywords) / sizeof(char *); i++)
    {
        size_t keywordLen = strlen(keywords[i]);
        if (strncmp(content, keywords[i], keywordLen) == 0)
        {
            // Honestly, why does strncmp return 0 on success
            token.tokenLength = keywordLen;
            return token;
        }
    }
    LEXER_TOKEN_CANNOT_CAPTURE();
}
LEXER_PARSER_FUNCTION(whitespace)
{
    LEXER_TOKEN_INIT_RESULT(TOKEN_TYPE_WHITESPACE);

    if (contentLength == 0)
    {
        LEXER_TOKEN_CANNOT_CAPTURE();
    }

    size_t i = 0;
    while (content[i] == ' ')
    {
        // Yep that's a space
        i++;
    }
    token.tokenLength = i;
    return token;
}
LEXER_PARSER_FUNCTION(newline)
{
    LEXER_TOKEN_INIT_RESULT(TOKEN_TYPE_NEWLINE);

    if (contentLength == 0)
    {
        LEXER_TOKEN_CANNOT_CAPTURE();
    }

    size_t i = 0;
    while (content[i] == '\n')
    {
        // Yep that's a newline
        i++;
    }
    token.tokenLength = i;
    return token;
}

LEXER_PARSER_FUNCTION(comment)
{
    // Comments start with /* and end with */
    LEXER_TOKEN_INIT_RESULT(TOKEN_TYPE_COMMENT);
    if (contentLength < 4)
    {
        LEXER_TOKEN_CANNOT_CAPTURE();
    }

    if (strncmp(content, "/*", 2) != 0)
    {
        LEXER_TOKEN_CANNOT_CAPTURE();
    }

    size_t i = 2;
    for (; i < contentLength; i++)
    {
        if (strncmp(content + i, "*/", 2) == 0)
        {
            break;
        }
    }

    if (i == contentLength)
    {
        // No closing comment
        LEXER_TOKEN_CANNOT_CAPTURE();
    }

    token.tokenLength = i + 2;
    return token;
}