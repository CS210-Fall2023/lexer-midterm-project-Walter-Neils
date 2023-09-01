#ifndef __LEXER_H__
#define __LEXER_H__
#include <string.h>
#include <stdlib.h>

#define LEXER_TOKEN_PARSE_HANDLER_CAPLEN_NAME(name) lexer_parser_##name##_capturable_length
#define LEXER_TOKEN_PARSE_HANDLER_PARSE_NAME(name) lexer_parser_##name##_parse

#define LEXER_TOKEN_PARSE_HANDLER_CAPLEN_DECL(name)                                                                    \
    int LEXER_TOKEN_PARSE_HANDLER_CAPLEN_NAME(name)(const char *text, int length)
#define LEXER_TOKEN_PARSE_HANDLER_PARSE_DECL(name)                                                                     \
    struct lexer_token *LEXER_TOKEN_PARSE_HANDLER_PARSE_NAME(name)(const char *text, int length)

#define LEXER_TOKEN_PARSE_HANDLER(name)                                                                                \
    LEXER_TOKEN_PARSE_HANDLER_CAPLEN_DECL(name);                                                                       \
    LEXER_TOKEN_PARSE_HANDLER_PARSE_DECL(name)

enum lexer_token_type
{
    LEXER_TOKEN_TYPE_ROOT,
    LEXER_TOKEN_TYPE_NONE,
    LEXER_TOKEN_TYPE_IDENTIFIER,
    LEXER_TOKEN_TYPE_NUMBER,
    LEXER_TOKEN_TYPE_STRING,
    LEXER_TOKEN_TYPE_OPERATOR,
    LEXER_TOKEN_TYPE_KEYWORD,
    LEXER_TOKEN_TYPE_COMMENT,
    LEXER_TOKEN_TYPE_NEWLINE,
    LEXER_TOKEN_TYPE_WHITESPACE,
    LEXER_TOKEN_TYPE_END_OF_FILE,
    LEXER_TOKEN_TYPE_UNKNOWN
};

struct lexer_state
{
    char *text;
    int currentPosition;
    int length;
    int childCount;
    struct lexer_token **children;
};

struct lexer
{
    struct lexer_state state;
};

struct lexer lexer_create(char *text);
struct lexer_token lexer_step(struct lexer *lexer);
struct lexer_token *lexer_token_parse_sub_tokens(struct lexer_token *token);
struct lexer_token *lexer_token_parse(const char *text, int length);

void lexer_state_add_child(struct lexer_state *lexer_state, struct lexer_token *child);

struct lexer_token
{
    enum lexer_token_type type;
    const char *text;
    int length;
    int childCount;
    struct lexer_token **children;
};

void lexer_token_initialize(struct lexer_token *token);
void lexer_token_child_add(struct lexer_token *token, struct lexer_token *child);
char *lexer_token_get_text(struct lexer_token *token);

LEXER_TOKEN_PARSE_HANDLER(number);
LEXER_TOKEN_PARSE_HANDLER(string);
LEXER_TOKEN_PARSE_HANDLER(whitespace);
LEXER_TOKEN_PARSE_HANDLER(newline);

#endif