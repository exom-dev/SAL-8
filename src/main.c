#include "sal8/sal8.h"

#include <stdio.h>
#include <stdlib.h>

#define VM_REG_COUNT 4
#define VM_STACK_SIZE 8

#define TERMINATE(code, fmt, ...) \
    do {\
        fprintf(stderr, "[error] " fmt "\n", ##__VA_ARGS__ ); \
        exit(code); \
    } while(0)

void run_file(const char* path);

int main(int argc, char** argv) {
    if(argc != 2)
        TERMINATE(1, "Usage: %s <FILE>", argv[0]);

    run_file(argv[1]);
}

void run_file(const char* path) {
    SAL8VM vm;
    SAL8Compiler compiler;
    SAL8Cluster cl;

    FILE *file = fopen(path, "rb");
    size_t size;
    SAL8CompilerStatus status;

    if(file == NULL)
        TERMINATE(1, "Cannot open file '%s'\n", path);

    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *src = SAL8_MEM_ALLOC(size + 1);

    if(fread(src, sizeof(char), size, file) < size) {
        fclose(file);
        SAL8_FREE(src);
        TERMINATE(1, "Cannot read file '%s'\n", path);
    }

    src[size] = '\0';

    fclose(file);

    sal8_compiler_init(&compiler, VM_REG_COUNT);

    status = sal8_compiler_compile(&compiler, src);

    cl = compiler.cl;

    sal8_compiler_delete(&compiler);

    SAL8_FREE(src);

    if(status == SAL8_COMPILER_ERROR) {
        sal8_cluster_delete(&cl);
        exit(1);
    }

    sal8_vm_init(&vm, VM_REG_COUNT, VM_STACK_SIZE);
    sal8_vm_load(&vm, cl);
    sal8_vm_run(&vm, SAL8_VM_RUNALL);
    sal8_vm_delete(&vm);

    sal8_cluster_delete(&cl);
}