#include "compiler.h"
#include "../mem/mem.h"

#include <string.h>
#include <stdlib.h>
#include <errno.h>

static void sal8_compiler_compile_label(SAL8Compiler* compiler);
static void sal8_compiler_compile_instruction(SAL8Compiler* compiler);

static void sal8_compiler_compile_binary(SAL8Compiler*);
static void sal8_compiler_compile_unary(SAL8Compiler*);
static void sal8_compiler_compile_jmp(SAL8Compiler*);
static void sal8_compiler_compile_cmp(SAL8Compiler*);
static void sal8_compiler_compile_out(SAL8Compiler*);

static uint8_t sal8_compiler_read_register(SAL8Compiler*);
static uint8_t sal8_compiler_read_number(SAL8Compiler*);
static uint8_t sal8_compiler_read_label(SAL8Compiler*);

static void sal8_compiler_emit_instruction(SAL8Compiler*, uint8_t, uint8_t, uint8_t);

void sal8_compiler_init(SAL8Compiler* compiler, uint8_t regCount) {
    compiler->regc = regCount;

    sal8_io_init(&compiler->io);
    sal8_cluster_init(&compiler->cl);
    sal8_label_map_init(&compiler->labels);

    compiler->ulabels = NULL;
}

void sal8_compiler_delete(SAL8Compiler* compiler) {
    sal8_label_map_delete(&compiler->labels);
}

void sal8_compiler_clean(SAL8Compiler* compiler) {
    sal8_label_map_delete(&compiler->labels);
    sal8_label_map_init(&compiler->labels);
    sal8_cluster_init(&compiler->cl);

    compiler->ulabels = NULL;
}

SAL8CompilerStatus sal8_compiler_compile(SAL8Compiler* compiler, const char* str) {
    SAL8Parser parser;
    bool firstInstruction = true;

    sal8_parser_init(&parser);
    sal8_io_clone(&parser.io, &compiler->io);

    compiler->parser = &parser;

    sal8_lexer_init(&compiler->parser->lexer);
    sal8_lexer_source(&compiler->parser->lexer, str);

    sal8_parser_advance(compiler->parser);

    while(compiler->parser->current.type != SAL8_TOKEN_EOF) {
        if(compiler->parser->current.type != SAL8_TOKEN_LINE && !firstInstruction) {
            sal8_parser_error_at_current(compiler->parser, "Expected new line");
            sal8_parser_advance(compiler->parser);
        } else {
            while(compiler->parser->current.type == SAL8_TOKEN_LINE)
                sal8_parser_advance(compiler->parser);

            if(compiler->parser->current.type == SAL8_TOKEN_EOF)
                break;

            if(compiler->parser->current.type == SAL8_TOKEN_LABEL)
                sal8_compiler_compile_label(compiler);
            else sal8_compiler_compile_instruction(compiler);
        }

        if(compiler->parser->panic)
            sal8_parser_sync(compiler->parser);

        if(firstInstruction)
            firstInstruction = false;
    }

    while(compiler->ulabels != NULL) {
        SAL8UnresolvedLabel* ptr = compiler->ulabels;
        sal8_parser_error_at(compiler->parser, ptr->token, "Undefined label");
        compiler->ulabels = ptr->link;
        SAL8_MEM_FREE(ptr);
    }

    return compiler->parser->error ? SAL8_COMPILER_ERROR : SAL8_COMPILER_OK;
}

static void sal8_compiler_compile_label(SAL8Compiler* compiler) {
    uint8_t offset;
    SAL8UnresolvedLabel* ulabel;

    if(sal8_label_map_nget(&compiler->labels, compiler->parser->current.start, compiler->parser->current.size - 1, &offset)) {
        sal8_parser_error_at_current(compiler->parser, "Label already defined");
        return;
    }

    if((compiler->cl.size / 3) > UINT8_MAX) {
        sal8_parser_error_at_current(compiler->parser, "Jump offset overflow");
        return;
    }

    if(!sal8_label_map_nset(&compiler->labels, compiler->parser->current.start, compiler->parser->current.size - 1, (uint8_t) (compiler->cl.size / 3))) {
        sal8_parser_error_at_current(compiler->parser, "PANIC: Cannot define label");
        return;
    }

    if((ulabel = compiler->ulabels) == NULL)
        goto _unresolved_label_end;

    // Check all nodes except the first.
    while(ulabel->link != NULL) {
        SAL8UnresolvedLabel* ptr = ulabel->link;

        if(ptr->token.size == compiler->parser->current.size - 1 && strncmp(compiler->parser->current.start, ptr->token.start, ptr->token.size) == 0) {
            compiler->cl.data[ptr->index + 1] = (uint8_t) (compiler->cl.size / 3);
            ulabel->link = ptr->link;
            SAL8_MEM_FREE(ptr);
            continue;
        }

        ulabel = ulabel->link;
    }

    ulabel = compiler->ulabels;

    // Check the first node.
    if(ulabel->token.size == compiler->parser->current.size - 1 && strncmp(compiler->parser->current.start, ulabel->token.start, ulabel->token.size) == 0) {
        compiler->cl.data[ulabel->index + 1] = (uint8_t) (compiler->cl.size / 3);
        compiler->ulabels = ulabel->link;
        SAL8_MEM_FREE(ulabel);
    }

_unresolved_label_end:
    sal8_parser_advance(compiler->parser);
}

static void  sal8_compiler_compile_instruction(SAL8Compiler* compiler) {
    switch(compiler->parser->current.type) {
        case SAL8_TOKEN_MOV:
        case SAL8_TOKEN_ADD:
        case SAL8_TOKEN_SUB:
        case SAL8_TOKEN_MUL:
        case SAL8_TOKEN_DIV:
        case SAL8_TOKEN_MOD:
        case SAL8_TOKEN_SHL:
        case SAL8_TOKEN_SHR:
        case SAL8_TOKEN_OR:
        case SAL8_TOKEN_AND:
        case SAL8_TOKEN_XOR:
            sal8_compiler_compile_binary(compiler);
            break;
        case SAL8_TOKEN_JMP:
        case SAL8_TOKEN_JE:
        case SAL8_TOKEN_JA:
        case SAL8_TOKEN_JAE:
        case SAL8_TOKEN_JB:
        case SAL8_TOKEN_JBE:
            sal8_compiler_compile_jmp(compiler);
            break;
        case SAL8_TOKEN_CMP:
            sal8_compiler_compile_cmp(compiler);
            break;
        case SAL8_TOKEN_NOT:
        case SAL8_TOKEN_POP:
        case SAL8_TOKEN_IN:
            sal8_compiler_compile_unary(compiler);
            break;
        case SAL8_TOKEN_PUSH:
        case SAL8_TOKEN_OUT:
            sal8_compiler_compile_out(compiler);
            break;
        default:
            sal8_parser_error_at_current(compiler->parser, "Expected instruction");
            sal8_parser_advance(compiler->parser);
            return;
    }
}

static void sal8_compiler_compile_binary(SAL8Compiler* compiler) {
    uint8_t op = sal8_token_to_opcode(compiler->parser->current.type);
    uint8_t right;
    uint8_t left;

    sal8_parser_advance(compiler->parser);

    if(compiler->parser->current.type != SAL8_TOKEN_REGISTER) {
        sal8_parser_error_at_current(compiler->parser, "Expected register");
        return;
    }

    left = sal8_compiler_read_register(compiler);

    if(compiler->parser->panic)
        return;

    sal8_parser_advance(compiler->parser);

    if(compiler->parser->current.type != SAL8_TOKEN_COMMA) {
        sal8_parser_error_at_current(compiler->parser, "Expected comma");
        return;
    }

    sal8_parser_advance(compiler->parser);

    if(compiler->parser->current.type == SAL8_TOKEN_REGISTER) {
        right = sal8_compiler_read_register(compiler);
        op |= SAL8_BYTECODE_RIGHT_TYPE_MASK;
    } else if(compiler->parser->current.type == SAL8_TOKEN_NUMBER) {
        right = sal8_compiler_read_number(compiler);
    } else {
        sal8_parser_error_at_current(compiler->parser, "Expected register or number");
        return;
    }

    if(compiler->parser->panic)
        return;

    sal8_parser_advance(compiler->parser);

    sal8_compiler_emit_instruction(compiler, op, left, right);
}

static void sal8_compiler_compile_unary(SAL8Compiler* compiler) {
    uint8_t op = sal8_token_to_opcode(compiler->parser->current.type);
    uint8_t left;

    sal8_parser_advance(compiler->parser);

    if(compiler->parser->current.type != SAL8_TOKEN_REGISTER) {
        sal8_parser_error_at_current(compiler->parser, "Expected register");
        return;
    }

    left = sal8_compiler_read_register(compiler);

    if(compiler->parser->panic)
        return;

    sal8_parser_advance(compiler->parser);

    sal8_compiler_emit_instruction(compiler, op, left, 0);
}

static void sal8_compiler_compile_jmp(SAL8Compiler* compiler) {
    uint8_t op = sal8_token_to_opcode(compiler->parser->current.type);
    uint8_t left;

    sal8_parser_advance(compiler->parser);

    if(compiler->parser->current.type != SAL8_TOKEN_IDENTIFIER) {
        sal8_parser_error_at_current(compiler->parser, "Expected label");
        return;
    }

    left = sal8_compiler_read_label(compiler);

    if(compiler->parser->panic)
        return;

    sal8_parser_advance(compiler->parser);

    sal8_compiler_emit_instruction(compiler, op, left, 0);
}

static void sal8_compiler_compile_cmp(SAL8Compiler* compiler) {
    uint8_t op = SAL8_OP_CMP;
    uint8_t left;
    uint8_t right;

    sal8_parser_advance(compiler->parser);

    if(compiler->parser->current.type == SAL8_TOKEN_REGISTER) {
        left = sal8_compiler_read_register(compiler);
        op |= SAL8_BYTECODE_LEFT_TYPE_MASK;
    } else if(compiler->parser->current.type == SAL8_TOKEN_NUMBER) {
        left = sal8_compiler_read_number(compiler);
    } else {
        sal8_parser_error_at_current(compiler->parser, "Expected register or number");
        return;
    }

    if(compiler->parser->panic)
        return;

    sal8_parser_advance(compiler->parser);

    if(compiler->parser->current.type != SAL8_TOKEN_COMMA) {
        sal8_parser_error_at_current(compiler->parser, "Expected comma");
        return;
    }

    sal8_parser_advance(compiler->parser);

    if(compiler->parser->current.type == SAL8_TOKEN_REGISTER) {
        right = sal8_compiler_read_register(compiler);
        op |= SAL8_BYTECODE_RIGHT_TYPE_MASK;
    } else if(compiler->parser->current.type == SAL8_TOKEN_NUMBER) {
        right = sal8_compiler_read_number(compiler);
    } else {
        sal8_parser_error_at_current(compiler->parser, "Expected register or number");
        return;
    }

    if(compiler->parser->panic)
        return;

    sal8_parser_advance(compiler->parser);

    sal8_compiler_emit_instruction(compiler, op, left, right);
}

static void sal8_compiler_compile_out(SAL8Compiler* compiler) {
    uint8_t op = sal8_token_to_opcode(compiler->parser->current.type);
    uint8_t left;

    sal8_parser_advance(compiler->parser);

    if(compiler->parser->current.type == SAL8_TOKEN_REGISTER) {
        left = sal8_compiler_read_register(compiler);
        op |= SAL8_BYTECODE_LEFT_TYPE_MASK;
    } else if(compiler->parser->current.type == SAL8_TOKEN_NUMBER) {
        left = sal8_compiler_read_number(compiler);
    } else {
        sal8_parser_error_at_current(compiler->parser, "Expected number or register");
        return;
    }

    if(compiler->parser->panic)
        return;

    sal8_parser_advance(compiler->parser);

    sal8_compiler_emit_instruction(compiler, op, left, 0);
}

static uint8_t sal8_compiler_read_register(SAL8Compiler* compiler) {
    int64_t num = strtol(compiler->parser->current.start, NULL, 10);

    if (errno == ERANGE || num < 0 || num >= compiler->regc) {
        sal8_parser_error_at_current(compiler->parser, "Unknown register");
        return UINT8_MAX;
    }

    return (uint8_t) num;
}

static uint8_t sal8_compiler_read_number(SAL8Compiler* compiler) {
    int64_t num = strtol(compiler->parser->current.start, NULL, 10);

    if (errno == ERANGE || num < 0 || num >= UINT8_MAX) {
        sal8_parser_error_at_current(compiler->parser, "Number out of range");
        return UINT8_MAX;
    }

    return (uint8_t) num;
}

static uint8_t sal8_compiler_read_label(SAL8Compiler* compiler) {
    uint8_t offset;

    // Undefined label.
    if(!sal8_label_map_nget(&compiler->labels, compiler->parser->current.start, compiler->parser->current.size, &offset)) {
        SAL8UnresolvedLabel* ulabel = SAL8_MEM_ALLOC(sizeof(SAL8UnresolvedLabel));
        ulabel->token = compiler->parser->current;
        ulabel->index = compiler->cl.size;

        ulabel->link = compiler->ulabels;
        compiler->ulabels = ulabel;

        offset = 0;
    }

    return offset;
}

static void sal8_compiler_emit_instruction(SAL8Compiler* compiler, uint8_t op, uint8_t left, uint8_t right) {
    sal8_cluster_write(&compiler->cl, &op, 1);
    sal8_cluster_write(&compiler->cl, &left, 1);
    sal8_cluster_write(&compiler->cl, &right, 1);
}