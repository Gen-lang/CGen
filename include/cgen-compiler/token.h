#ifndef __CGEN_COMPILER_TOKEN
#define __CGEN_COMPILER_TOKEN

#include <stddef.h>

typedef enum CGEN_TOKEN_ENUM
{
    CGEN_TOKEN_IDENTIFIER,
    CGEN_TOKEN_KEYWORD,
    CGEN_TOKEN_STRING,
    CGEN_TOKEN_NIL,
    CGEN_TOKEN_INT,
    CGEN_TOKEN_FLOAT,

    CGEN_TOKEN_SEMICOLON,
    CGEN_TOKEN_PARENTHESIS_0,
    CGEN_TOKEN_PARENTHESIS_1,
    CGEN_TOKEN_BRACKET_0,
    CGEN_TOKEN_BRACKET_1,
    CGEN_TOKEN_COMMA,

    CGEN_TOKEN_EQUAL,
    CGEN_TOKEN_NON_EQUAL,
    CGEN_TOKEN_ASSIGN,
    CGEN_TOKEN_EQUAL_GREATER,
    CGEN_TOKEN_EQUAL_LESSER,
    CGEN_TOKEN_GREATER,
    CGEN_TOKEN_LESSER,
    CGEN_TOKEN_AT,

    CGEN_TOKEN_ADD,
    CGEN_TOKEN_SUB,
    CGEN_TOKEN_MUL,
    CGEN_TOKEN_DIV,
    CGEN_TOKEN_POW,
    CGEN_TOKEN_MOD,

    CGEN_TOKEN_EOF,
} CGEN_TOKEN_ENUM;

/* CGen token structure */
typedef struct CGEN_TOKEN_STRUCT
{
    /* CGen token type | enum */
    CGEN_TOKEN_ENUM token_type;
    /* CGen token int | 64 bit integer */
    long long int token_int;
    /* CGen token double | 64 bit float */
    double token_double;
    /* CGen token string | array of characters */
    char* token_string;

    /* CGen token size | size_t */
    size_t token_size;    
} CGEN_TOKEN_STRUCT;

#endif