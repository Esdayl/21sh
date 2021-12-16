#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "alloc.h"

static struct token *lexer_search(struct lexer *lexer);
static ssize_t lexer_number(struct lexer *lexer, char c);

// Skip all spaces
static inline void lexer_skip_spaces(struct lexer *lexer)
{
    while (lexer->input[lexer->pos] == ' ')
        lexer->pos++;
}

// Create a new lexer
struct lexer *lexer_new(const char *input)
{
    struct lexer *new = xmalloc(sizeof(struct lexer));
    new->input = input;
    new->pos = 0;
    new->current_tok = NULL;
    return new;
}

// Free the lexer
void lexer_free(struct lexer *lexer)
{
    if (lexer)
    {
        // token_free(lexer->current_tok);
        free(lexer);
        lexer = NULL;
    }
}

// Return the current token without change it
struct token *lexer_peek(struct lexer *lexer)
{
    if (lexer->current_tok)
        return lexer->current_tok;
    struct token *token = lexer_search(lexer);
    lexer->current_tok = token;
    return token;
}

// Return the current token and get the next one
struct token *lexer_pop(struct lexer *lexer)
{
    struct token *token = lexer_search(lexer);
    struct token *res = token;
    if (lexer->current_tok)
    {
        res = lexer->current_tok;
        lexer->current_tok = token;
    }
    return res;
}

// Find number in string
static ssize_t lexer_number(struct lexer *lexer, char c)
{
    char *str = xmalloc(2);
    str[0] = c;
    c = lexer->input[++(lexer->pos)];

    size_t i = 1;
    while (c >= '0' && c <= '9')
    {
        str[i++] = c;
        str = xrealloc(str, i + 1);
        c = lexer->input[++(lexer->pos)];
    }
    str[i] = '\0';

    ssize_t val = strtoul(str, NULL, 10);
    free(str);
    str = NULL;

    return val;
}

// Distinguish each kind of token
struct token *lexer_search(struct lexer *lexer)
{
    lexer_skip_spaces(lexer);
    char c = lexer->input[lexer->pos];
    enum token_type type = TOKEN_ERROR;
    ssize_t val;

    if (c == '+')
        type = TOKEN_PLUS;
    else if (c == '-')
        type = TOKEN_MINUS;
    else if (c == '*')
        type = TOKEN_MUL;
    else if (c == '/')
        type = TOKEN_DIV;
    else if (c == '(')
        type = TOKEN_LEFT_PAR;
    else if (c == ')')
        type = TOKEN_RIGHT_PAR;
    else if (c == '\0')
        type = TOKEN_EOF;
    else if (c >= '0' && c <= '9')
    {
        type = TOKEN_NUMBER;
        val = lexer_number(lexer, c);
    }
    else
        fprintf(stderr, "lexer: token %c is not valid\n", c);

    struct token *token = token_new(type);

    if (type == TOKEN_NUMBER)
        token->value = val;
    else
        lexer->pos++;

    return token;
}