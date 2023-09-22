#include "lexer.h"
const char *tokenTypeToString(enum TOKEN_TYPE token)
{
    const char *values[] = {"UNKNOWN", "IDENTIFIER", "NUMBER",  "STRING",     "CHARACTER_LITERAL", "OPERATOR",
                            "KEYWORD", "COMMENT",    "NEWLINE", "WHITESPACE", "PUNCTUATION",       "END_OF_FILE"};
    return values[token];
}
struct lexer_token_parser
{
    struct lexer_token (*parser)(const char *content, size_t contentLength);
};
struct lexer_token lexer_tokenize(const char *content, size_t contentLength)
{
    if (contentLength == 0)
    {
        struct lexer_token token;
        token.token = content;
        token.tokenLength = 0;
        token.type = TOKEN_TYPE_END_OF_FILE;
        return token;
    }
    struct lexer_token_parser parsers[] = {{commentParser},    {keywordParser},    {stringParser},
                                           {numberParser},     {operatorParser},   {characterLiteralParser},
                                           {identifierParser}, {whitespaceParser}, {newlineParser}};
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
        LEXER_TOKEN_CANNOT_CAPTURE();
    }
    if (content[0] != '"')
    {
        LEXER_TOKEN_CANNOT_CAPTURE();
    }
    size_t i = 1;
    for (; i < contentLength; i++)
    {
        if (content[i] == '"')
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
LEXER_PARSER_FUNCTION(number)
{
    LEXER_TOKEN_INIT_RESULT(TOKEN_TYPE_NUMBER);
    if (contentLength == 0)
    {
        LEXER_TOKEN_CANNOT_CAPTURE();
    }
    size_t i = 0;
    bool isNegative = false;
    if (content[0] == '-')
    {
        isNegative = true;
        i++;
    }
    bool hasDecimalPoint = false;
    for (; i < contentLength; i++)
    {
        // Number can contain digits, and a single decimal point
        if (content[i] == '.' && i != 0)
        {
            if (hasDecimalPoint)
            {
                // Number can only have one decimal point
                LEXER_TOKEN_CANNOT_CAPTURE();
            }
            else
            {
                hasDecimalPoint = true;
            }
        }
        else if (content[i] < '0' || content[i] > '9')
        {
            break;
        }
    }
    if (i == 0)
    {
        LEXER_TOKEN_CANNOT_CAPTURE();
    }
    if (i == 1 && isNegative)
    {
        LEXER_TOKEN_CANNOT_CAPTURE();
    }
    token.tokenLength = i;
    return token;
}
LEXER_PARSER_FUNCTION(operator)
{
    // Possible tokens:
    /**
     * “.”   “<”   “>”   “(“   “)”   “+”   “-“   “*”   “/”   “|”   “&”   “;”   “,”   “:”
“[“   “]”  “=”   “:=”   “..”   “<<”   “>>”   “<>”   “<=”   “>=”   “**”   “!=”   “=>”
    */

    const char *possibleOperatorTokens[] = {
        ":=", "..", "<<", ">>", "<>", "<=", ">=", "**", "!=", "=>", ".", "<", ">", "(",
        ")",  "+",  "-",  "*",  "/",  "|",  "&",  ";",  ",",  ":",  "[", "]", "="};

    LEXER_TOKEN_INIT_RESULT(TOKEN_TYPE_OPERATOR);
    if (contentLength == 0)
    {
        LEXER_TOKEN_CANNOT_CAPTURE();
    }

    for (size_t i = 0; i < sizeof(possibleOperatorTokens) / sizeof(char *); i++)
    {
        if (strncmp(content, possibleOperatorTokens[i], strlen(possibleOperatorTokens[i])) == 0)
        {
            token.tokenLength = strlen(possibleOperatorTokens[i]);
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
        LEXER_TOKEN_CANNOT_CAPTURE();
    }
    if (content[0] != '\'')
    {
        LEXER_TOKEN_CANNOT_CAPTURE();
    }
    size_t i = 1;
    for (; i < contentLength; i++)
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
    // List of valid keywords:
    /**
     * accessor and array begin bool case character constant else elsif end exit function
if in integer interface is loop module mutator natural null of or other out
positive procedure range return struct subtype then type when while
    */
    LEXER_TOKEN_INIT_RESULT(TOKEN_TYPE_KEYWORD);
    if (contentLength == 0)
    {
        LEXER_TOKEN_CANNOT_CAPTURE();
    }
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
            // printf("Found keyword '%s'\n", keywords[i]);
            token.tokenLength = keywordLen;
            return token;
        }
        else
        {
            // printf("Keyword '%s' does not match '%.*s'\n", keywords[i], contentLength, content);
        }
    }
    LEXER_TOKEN_CANNOT_CAPTURE();
}
LEXER_PARSER_FUNCTION(whitespace)
{
    LEXER_TOKEN_INIT_RESULT(TOKEN_TYPE_WHITESPACE);
    size_t i = 0;
    while (content[i] == ' ')
    {
        i++;
    }
    token.tokenLength = i;
    return token;
}
LEXER_PARSER_FUNCTION(newline)
{
    LEXER_TOKEN_INIT_RESULT(TOKEN_TYPE_NEWLINE);
    size_t i = 0;
    while (content[i] == '\n')
    {
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