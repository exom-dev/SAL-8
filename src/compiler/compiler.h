#ifndef SAL8_COMPILER_H_GUARD
#define SAL8_COMPILER_H_GUARD

#include "parser.h"
#include "../io/io.h"
#include "../lexer/lexer.h"
#include "../cluster/cluster.h"
#include "../data/label_map.h"

typedef struct {
    SAL8IO io;

    uint8_t regc;

    SAL8Cluster cl;

    SAL8Parser* parser;
    SAL8LabelMap labels;
} SAL8Compiler;

typedef enum {
    SAL8_COMPILER_OK,
    SAL8_COMPILER_ERROR
} SAL8CompilerStatus;

void sal8_compiler_init(SAL8Compiler* compiler, uint8_t regCount);
void sal8_compiler_delete(SAL8Compiler* compiler);

SAL8CompilerStatus sal8_compiler_compile(SAL8Compiler* compiler, const char* str);

#endif
