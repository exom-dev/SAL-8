#include "lexer.h"
#include "../lib/charlib.h"

#include <string.h>

#define PEEK(i) lexer->current[i]
#define ADVANCE(i) (++lexer->index, lexer->current += i)
#define NEXT() (++lexer->index, *(lexer->current++))

#define AT_END(i) (lexer->current[i] == '\0')
#define MATCH(c) \
    ((AT_END(0) || *lexer->current != c) ? false : (ADVANCE(1), true))

SAL8Token sal8_lexer_next_identifier(SAL8Lexer* lexer);
SAL8Token sal8_lexer_next_number(SAL8Lexer* lexer);
SAL8Token sal8_lexer_next_register(SAL8Lexer* lexer);

SAL8OpCode sal8_token_to_opcode(SAL8TokenType token) {
    switch(token) {
        case SAL8_TOKEN_MOV:  return SAL8_OP_MOV;
        case SAL8_TOKEN_PUSH: return SAL8_OP_PUSH;
        case SAL8_TOKEN_POP:  return SAL8_OP_POP;
        case SAL8_TOKEN_JMP:  return SAL8_OP_JMP;
        case SAL8_TOKEN_JE:   return SAL8_OP_JE;
        case SAL8_TOKEN_JA:   return SAL8_OP_JA;
        case SAL8_TOKEN_JAE:  return SAL8_OP_JAE;
        case SAL8_TOKEN_JB:   return SAL8_OP_JB;
        case SAL8_TOKEN_JBE:  return SAL8_OP_JBE;
        case SAL8_TOKEN_CMP:  return SAL8_OP_CMP;
        case SAL8_TOKEN_ADD:  return SAL8_OP_ADD;
        case SAL8_TOKEN_SUB:  return SAL8_OP_SUB;
        case SAL8_TOKEN_MUL:  return SAL8_OP_MUL;
        case SAL8_TOKEN_DIV:  return SAL8_OP_DIV;
        case SAL8_TOKEN_MOD:  return SAL8_OP_MOD;
        case SAL8_TOKEN_SHL:  return SAL8_OP_SHL;
        case SAL8_TOKEN_SHR:  return SAL8_OP_SHR;
        case SAL8_TOKEN_NOT:  return SAL8_OP_NOT;
        case SAL8_TOKEN_OR:   return SAL8_OP_OR;
        case SAL8_TOKEN_AND:  return SAL8_OP_AND;
        case SAL8_TOKEN_XOR:  return SAL8_OP_XOR;
        case SAL8_TOKEN_IN:   return SAL8_OP_IN;
        case SAL8_TOKEN_OUT:  return SAL8_OP_OUT;
        default:              return SAL8_OP_MOV;
    }
}

void sal8_lexer_init(SAL8Lexer* lexer) {
    lexer->source = NULL;
    lexer->start = NULL;
    lexer->current = NULL;
    lexer->index = 0;
}

void sal8_lexer_source(SAL8Lexer* lexer, const char* src) {
    lexer->source = src;
    lexer->start = src;
    lexer->current = src;
}

SAL8Token sal8_lexer_next(SAL8Lexer* lexer) {
    bool whitespace = true;

    while(!AT_END(0) && whitespace) {
        switch(PEEK(0)) {
            case ' ':
            case '\t':
            case '\r':
                ADVANCE(1);
                break;
            case '/':
                if(PEEK(1) == '/') {
                    ADVANCE(2);

                    while(!AT_END(0) && PEEK(0) != '\n')
                        ADVANCE(1);
                } else if(PEEK(1) == '*') {
                    ADVANCE(2);

                    while(!AT_END(0)) {
                        if(PEEK(0) == '*' && !AT_END(1) && PEEK(1) == '/')
                            break;
                        ADVANCE(1);
                    }

                    if(AT_END(0))
                        return sal8_lexer_error(lexer, "Expected end of comment block");

                    ADVANCE(2);
                } else whitespace = false;

                break;
            default:
                whitespace = false;
        }
    }

    lexer->start = lexer->current;

    if(AT_END(0))
        return sal8_lexer_emit(lexer, SAL8_TOKEN_EOF);

    char c = NEXT();

    switch(c) {
        case '\n':
            return sal8_lexer_emit(lexer, SAL8_TOKEN_LINE);
        case ',':
            return sal8_lexer_emit(lexer, SAL8_TOKEN_COMMA);
        case 'r': case 'R':
            lexer->start = lexer->current; // Skip the 'R' prefix.
            return sal8_lexer_next_register(lexer);
        default:
            break;
    }

    if(sal8_lib_charlib_is_alpha(c))
        return sal8_lexer_next_identifier(lexer);
    if(sal8_lib_charlib_is_digit(c))
        return sal8_lexer_next_number(lexer);

    return sal8_lexer_error(lexer, "Unexpected character");
}

SAL8Token sal8_lexer_emit(SAL8Lexer* lexer, SAL8TokenType type) {
    SAL8Token token;
    token.type = type;
    token.start = lexer->start;
    token.size = (size_t) (lexer->current - lexer->start);
    token.index = lexer->index - token.size;

    return token;
}

SAL8Token sal8_lexer_error(SAL8Lexer* lexer, const char* msg) {
    SAL8Token token;
    token.type = SAL8_TOKEN_ERROR;
    token.start = msg;
    token.size = strlen(msg);
    token.index = lexer->index;

    return token;
}

SAL8Token sal8_lexer_next_identifier(SAL8Lexer* lexer) {
    #define IMATCH_INSTRUCTION(index, length, str) \
            ((lexer->current - lexer->start == index + length) && \
            (sal8_lib_charlib_strnicmp(lexer->start + index, str, length)))

    while(!AT_END(0) && (sal8_lib_charlib_is_alpha(PEEK(0)) || sal8_lib_charlib_is_digit(PEEK(0))))
        ADVANCE(1);

    if(MATCH(':'))
        return sal8_lexer_emit(lexer, SAL8_TOKEN_LABEL);

    switch(*lexer->start) {
        case 'a': case 'A':
            if(lexer->current - lexer->start > 1) {
                switch(lexer->start[1]) {
                    case 'd': case 'D': return sal8_lexer_emit(lexer, IMATCH_INSTRUCTION(2, 1, "d") ? SAL8_TOKEN_ADD : SAL8_TOKEN_IDENTIFIER);
                    case 'n': case 'N': return sal8_lexer_emit(lexer, IMATCH_INSTRUCTION(2, 1, "d") ? SAL8_TOKEN_AND : SAL8_TOKEN_IDENTIFIER);
                }
            }
            return sal8_lexer_emit(lexer, SAL8_TOKEN_IDENTIFIER);
        case 'c': case 'C':
            return sal8_lexer_emit(lexer, IMATCH_INSTRUCTION(1, 2, "mp") ? SAL8_TOKEN_CMP : SAL8_TOKEN_IDENTIFIER);
        case 'd': case 'D':
            return sal8_lexer_emit(lexer, IMATCH_INSTRUCTION(1, 2, "iv") ? SAL8_TOKEN_DIV : SAL8_TOKEN_IDENTIFIER);
        case 'i': case 'I':
            return sal8_lexer_emit(lexer, IMATCH_INSTRUCTION(1, 1, "n") ? SAL8_TOKEN_IN : SAL8_TOKEN_IDENTIFIER);
        case 'j': case 'J':
            if(lexer->current - lexer->start > 1) {
                switch(lexer->start[1]) {
                    case 'a': case 'A': return sal8_lexer_emit(lexer, IMATCH_INSTRUCTION(2, 1, "e") ? SAL8_TOKEN_JAE :
                                                                      ((lexer->current - lexer->start == 2) ? SAL8_TOKEN_JA : SAL8_TOKEN_IDENTIFIER));
                    case 'b': case 'B': return sal8_lexer_emit(lexer, IMATCH_INSTRUCTION(2, 1, "e") ? SAL8_TOKEN_JBE :
                                                                      ((lexer->current - lexer->start == 2) ? SAL8_TOKEN_JB : SAL8_TOKEN_IDENTIFIER));
                    case 'e': case'E': return sal8_lexer_emit(lexer, (lexer->current - lexer->start == 2) ? SAL8_TOKEN_JE : SAL8_TOKEN_IDENTIFIER);
                    case 'm': case 'M': return sal8_lexer_emit(lexer, IMATCH_INSTRUCTION(2, 1, "p") ? SAL8_TOKEN_JMP : SAL8_TOKEN_IDENTIFIER);
                }
            }
            return sal8_lexer_emit(lexer, SAL8_TOKEN_IDENTIFIER);
        case 'm': case 'M':
            if(lexer->current - lexer->start > 1) {
                switch(lexer->start[1]) {
                    case 'o': case 'O': return sal8_lexer_emit(lexer, IMATCH_INSTRUCTION(2, 1, "v") ? SAL8_TOKEN_MOV :
                                                                      (IMATCH_INSTRUCTION(2, 1, "d") ? SAL8_TOKEN_MOD : SAL8_TOKEN_IDENTIFIER));

                }
            }
            return sal8_lexer_emit(lexer, SAL8_TOKEN_IDENTIFIER);
        case 'n': case 'N':
            return sal8_lexer_emit(lexer, IMATCH_INSTRUCTION(1, 2, "ot") ? SAL8_TOKEN_NOT : SAL8_TOKEN_IDENTIFIER);
        case 'o': case 'O':
            if(lexer->current - lexer->start > 1) {
                switch(lexer->start[1]) {
                    case 'r': case 'R': return sal8_lexer_emit(lexer, (lexer->current - lexer->start == 2) ? SAL8_TOKEN_OR : SAL8_TOKEN_IDENTIFIER);
                    case 'u': case 'U': return sal8_lexer_emit(lexer, IMATCH_INSTRUCTION(2, 1, "t") ? SAL8_TOKEN_OUT : SAL8_TOKEN_IDENTIFIER);
                }
            }
            return sal8_lexer_emit(lexer, SAL8_TOKEN_IDENTIFIER);
        case 'p': case 'P':
            if(lexer->current - lexer->start > 1) {
                switch(lexer->start[1]) {
                    case 'o': case 'O': return sal8_lexer_emit(lexer, IMATCH_INSTRUCTION(2, 1, "p") ? SAL8_TOKEN_POP : SAL8_TOKEN_IDENTIFIER);
                    case 'u': case 'U': return sal8_lexer_emit(lexer, IMATCH_INSTRUCTION(2, 2, "sh") ? SAL8_TOKEN_PUSH : SAL8_TOKEN_IDENTIFIER);
                }
            }
            return sal8_lexer_emit(lexer, SAL8_TOKEN_IDENTIFIER);
        case 's': case 'S':
            if(lexer->current - lexer->start > 1) {
                switch(lexer->start[1]) {
                case 'u': case 'U':     return sal8_lexer_emit(lexer, IMATCH_INSTRUCTION(2, 1, "b") ? SAL8_TOKEN_SUB : SAL8_TOKEN_IDENTIFIER);
                    case 'h': case 'H': return sal8_lexer_emit(lexer, IMATCH_INSTRUCTION(2, 1, "l") ? SAL8_TOKEN_SHL :
                                                                      (IMATCH_INSTRUCTION(2, 1, "r") ? SAL8_TOKEN_SHR : SAL8_TOKEN_IDENTIFIER));
                }
            }
            return sal8_lexer_emit(lexer, SAL8_TOKEN_IDENTIFIER);
        case 'x': case 'X':
            return sal8_lexer_emit(lexer, IMATCH_INSTRUCTION(1, 2, "or") ? SAL8_TOKEN_XOR : SAL8_TOKEN_IDENTIFIER);
        default: return sal8_lexer_emit(lexer, SAL8_TOKEN_IDENTIFIER);
    }

    #undef IMATCH_INSTRUCTION
}

SAL8Token sal8_lexer_next_number(SAL8Lexer* lexer) {
    while(!AT_END(0) && sal8_lib_charlib_is_digit(PEEK(0)))
        ADVANCE(1);

    return sal8_lexer_emit(lexer, SAL8_TOKEN_NUMBER);
}

SAL8Token sal8_lexer_next_register(SAL8Lexer* lexer) {
    while(!AT_END(0) && sal8_lib_charlib_is_digit(PEEK(0)))
        ADVANCE(1);

    return sal8_lexer_emit(lexer, SAL8_TOKEN_REGISTER);
}

#undef PEEK
#undef ADVANCE
#undef NEXT

#undef AT_END
#undef MATCH