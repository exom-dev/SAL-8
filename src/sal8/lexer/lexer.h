#ifndef SAL8_LEXER_H_GUARD
#define SAL8_LEXER_H_GUARD

#include "../cluster/bytecode.h"

#include "../api.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    SAL8_TOKEN_LABEL, SAL8_TOKEN_IDENTIFIER, SAL8_TOKEN_NUMBER, SAL8_TOKEN_REGISTER,

    SAL8_TOKEN_MOV, SAL8_TOKEN_PUSH, SAL8_TOKEN_POP,
    SAL8_TOKEN_JMP, SAL8_TOKEN_CMP, SAL8_TOKEN_JE, SAL8_TOKEN_JA, SAL8_TOKEN_JAE, SAL8_TOKEN_JB, SAL8_TOKEN_JBE,

    SAL8_TOKEN_ADD, SAL8_TOKEN_SUB, SAL8_TOKEN_MUL, SAL8_TOKEN_DIV, SAL8_TOKEN_MOD,
    SAL8_TOKEN_SHL, SAL8_TOKEN_SHR, SAL8_TOKEN_NOT, SAL8_TOKEN_OR, SAL8_TOKEN_AND, SAL8_TOKEN_XOR,

    SAL8_TOKEN_IN, SAL8_TOKEN_OUT,

    SAL8_TOKEN_LINE, SAL8_TOKEN_COMMA,
    SAL8_TOKEN_ERROR,
    SAL8_TOKEN_EOF
} SAL8TokenType;

typedef struct {
    SAL8TokenType type;
    const char* start;

    uint32_t size;
    uint32_t index;
} SAL8Token;

typedef struct {
    const char* source;
    const char* start;
    const char* current;

    uint32_t index;
} SAL8Lexer;

SAL8_API SAL8OpCode sal8_token_to_opcode(SAL8TokenType token);

SAL8_API void sal8_lexer_init(SAL8Lexer* lexer);
SAL8_API void sal8_lexer_source(SAL8Lexer* lexer, const char* src);

SAL8_API SAL8Token sal8_lexer_next(SAL8Lexer* lexer);
SAL8_API SAL8Token sal8_lexer_emit(SAL8Lexer* lexer, SAL8TokenType type);
SAL8_API SAL8Token sal8_lexer_error(SAL8Lexer* lexer, const char* msg);

#endif
