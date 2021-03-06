#ifndef SAL8_COMPILER_H_GUARD
#define SAL8_COMPILER_H_GUARD

#include "../api.h"
#include "parser.h"
#include "../io/io.h"
#include "../lexer/lexer.h"
#include "../cluster/cluster.h"
#include "../data/label_map.h"

typedef struct SAL8UnresolvedLabel {
    struct SAL8UnresolvedLabel* link;
    SAL8Token token;
    uint8_t index;
} SAL8UnresolvedLabel;

typedef struct {
    SAL8IO io;

    uint8_t regc;

    SAL8Cluster cl;

    SAL8Parser* parser;
    SAL8LabelMap labels;
    SAL8UnresolvedLabel* ulabels;
} SAL8Compiler;

typedef enum {
    SAL8_COMPILER_OK,
    SAL8_COMPILER_ERROR
} SAL8CompilerStatus;

SAL8_API void sal8_compiler_init(SAL8Compiler* compiler, uint8_t regCount);
SAL8_API void sal8_compiler_delete(SAL8Compiler* compiler);
SAL8_API void sal8_compiler_clean(SAL8Compiler* compiler);

SAL8_API SAL8CompilerStatus sal8_compiler_compile(SAL8Compiler* compiler, const char* str);

#endif
