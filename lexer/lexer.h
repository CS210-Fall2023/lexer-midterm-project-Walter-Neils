#ifndef __LEXER_H__
#define __LEXER_H__
#include <stdlib.h>
#include <stdio.h>
#define bool int
#define true 1
#define false 0

enum TOKEN_TYPE
{
    TOKEN_TYPE_UNKNOWN = 0,
    TOKEN_TYPE_IDENTIFIER,
    TOKEN_TYPE_NUMBER,
    TOKEN_TYPE_STRING,
    TOKEN_TYPE_CHARACTER_LITERAL,
    TOKEN_TYPE_OPERATOR,
    TOKEN_TYPE_KEYWORD,
    TOKEN_TYPE_COMMENT,
    TOKEN_TYPE_NEWLINE,
    TOKEN_TYPE_WHITESPACE,
    TOKEN_TYPE_PUNCTUATION,
    TOKEN_TYPE_END_OF_FILE
};

struct lexer_token
{
    char *token;
    size_t tokenLength;
    enum TOKEN_TYPE type;
};

#define LEXER_PARSER_FUNCTION_NAME(name) name##Parser
#define LEXER_PARSER_FUNCTION(name)                                                                                    \
    struct lexer_token LEXER_PARSER_FUNCTION_NAME(name)(const char *content, size_t contentLength)

#define LEXER_TOKEN_INIT_RESULT(tokenType)                                                                             \
    struct lexer_token token;                                                                                          \
    token.type = tokenType;                                                                                            \
    token.token = content;                                                                                             \
    token.tokenLength = 0;

#define LEXER_TOKEN_CANNOT_CAPTURE()                                                                                   \
    token.tokenLength = 0;                                                                                             \
    return token;

struct lexer_token lexer_tokenize(const char *content, size_t contentLength);

LEXER_PARSER_FUNCTION(string);
LEXER_PARSER_FUNCTION(number);
LEXER_PARSER_FUNCTION(operator);
LEXER_PARSER_FUNCTION(characterLiteral);
LEXER_PARSER_FUNCTION(identifier);
LEXER_PARSER_FUNCTION(keyword);
LEXER_PARSER_FUNCTION(whitespace);
LEXER_PARSER_FUNCTION(newline);
LEXER_PARSER_FUNCTION(comment);

#endif