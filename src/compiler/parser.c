#include "parser.h"
#include "../io/log.h"
#include "../lib/mem_index.h"

void sal8_parser_init(SAL8Parser* parser) {
    sal8_io_init(&parser->io);
    parser->error = false;
    parser->panic = false;
}

void sal8_parser_advance(SAL8Parser* parser) {
    parser->previous = parser->current;

    while(1) {
        parser->current = sal8_lexer_next(&parser->lexer);

        if(parser->current.type != SAL8_TOKEN_ERROR)
            break;

        sal8_parser_error_at_current(parser, parser->current.start);
    }
}

void sal8_parser_consume(SAL8Parser* parser, SAL8TokenType type, const char* err) {
    if(parser->current.type == type) {
        sal8_parser_advance(parser);
        return;
    }

    sal8_parser_error_at_current(parser, err);
}

void sal8_parser_sync(SAL8Parser* parser) {
    parser->panic = false;

    while(parser->current.type != SAL8_TOKEN_EOF) {
        switch(parser->current.type) {
            case SAL8_TOKEN_REGISTER:
            case SAL8_TOKEN_NUMBER:
            case SAL8_TOKEN_LABEL:
            case SAL8_TOKEN_IDENTIFIER:
                sal8_parser_advance(parser);
                continue;
            default: return;
        }
    }
}

void sal8_parser_error_at(SAL8Parser* parser, SAL8Token token, const char* msg) {
    if(parser->panic)
        return;
    parser->panic = true;

    if(token.type == SAL8_TOKEN_EOF)
        SAL8_ERROR(parser->io, "at EOF: %s\n", msg);
    else if(token.type != SAL8_TOKEN_ERROR) {
        size_t ln;
        size_t col;

        sal8_lib_mem_lncol(parser->lexer.source, token.index, &ln, &col);
        SAL8_ERROR(parser->io, "at %zu:%zu in script: %s\n", ln, col, msg);
    }

    parser->error = true;
}

void sal8_parser_error_at_current(SAL8Parser* parser, const char* msg) {
    sal8_parser_error_at(parser, parser->current, msg);
}

void sal8_parser_error_at_previous(SAL8Parser* parser, const char* msg) {
    sal8_parser_error_at(parser, parser->previous, msg);
}