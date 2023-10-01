#ifndef __LEXER_H__
#define __LEXER_H__
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// The fact that I have to do this is downright hilarious
#define bool int
#define true 1
#define false 0

// List of all possible token types
// Some aren't displayed, but useful to capture them to keep lexer behaviour indiscriminate
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

/**
 * @brief A token returned by the lexer
 */
struct lexer_token
{
    /**
     * @brief A pointer to the start of the token. This is a pointer to the original string passed to the lexer.
     */
    const char *token;
    /**
     * @brief The length of the token
     */
    size_t tokenLength;
    /**
     * @brief The type of the token
     */
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

/**
 * @brief Tokenizes a string
 * @param content The string to tokenize
 * @param contentLength The length of the string to tokenize
 * @returns A token struct
 */
struct lexer_token lexer_tokenize(const char *content, size_t contentLength);
/*
 * @brief Prints a token to a stream
 * @param token The token to print
 * @param outputStream The stream to print to
 */
void printToken(struct lexer_token *token, FILE *outputStream);

/*
 * @brief Parses a string literal
 * @param content The string to parse
 * @param contentLength The length of the string to try to parse
 */
LEXER_PARSER_FUNCTION(string);
/*
 * @brief Parses a number literal
 * @param content The string to parse
 * @param contentLength The length of the string to try to parse
 */
LEXER_PARSER_FUNCTION(number);
/*
 * @brief Parses an operator
 * @param content The string to parse
 * @param contentLength The length of the string to try to parse
 */
LEXER_PARSER_FUNCTION(operator);
/*
 * @brief Parses a character literal
 * @param content The string to parse
 * @param contentLength The length of the string to try to parse
 */
LEXER_PARSER_FUNCTION(characterLiteral);
/*
 * @brief Parses an identifier
 * @param content The string to parse
 * @param contentLength The length of the string to try to parse
 */
LEXER_PARSER_FUNCTION(identifier);
/*
 * @brief Parses a keyword
 * @param content The string to parse
 * @param contentLength The length of the string to try to parse
 */
LEXER_PARSER_FUNCTION(keyword);
/*
 * @brief Parses a whitespace character
 * @param content The string to parse
 * @param contentLength The length of the string to try to parse
 */
LEXER_PARSER_FUNCTION(whitespace);
/*
 * @brief Parses a newline character
 * @param content The string to parse
 * @param contentLength The length of the string to try to parse
 */
LEXER_PARSER_FUNCTION(newline);
/*
 * @brief Parses a comment
 * @param content The string to parse
 * @param contentLength The length of the string to try to parse
 */
LEXER_PARSER_FUNCTION(comment);

#endif