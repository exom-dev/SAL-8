#ifndef SAL8_PARSER_H_GUARD
#define SAL8_PARSER_H_GUARD

#include "../api.h"
#include "../lexer/lexer.h"
#include "../io/io.h"

typedef struct {
    SAL8IO io;
    SAL8Lexer lexer;

    SAL8Token current;
    SAL8Token previous;

    bool error;
    bool panic;
} SAL8Parser;

SAL8_API void sal8_parser_init(SAL8Parser* parser);
SAL8_API void sal8_parser_advance(SAL8Parser* parser);
SAL8_API void sal8_parser_consume(SAL8Parser* parser, SAL8TokenType type, const char* err);

SAL8_API void sal8_parser_sync(SAL8Parser* parser);

SAL8_API void sal8_parser_error_at(SAL8Parser* parser, SAL8Token token, const char* msg);
SAL8_API void sal8_parser_error_at_current(SAL8Parser* parser, const char* msg);
SAL8_API void sal8_parser_error_at_previous(SAL8Parser* parser, const char* msg);


#endif
