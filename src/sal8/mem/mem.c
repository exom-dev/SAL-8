#include "mem.h"
#include "../io/log.h"

#include <stdlib.h>

#define MEM_BLOCK_START_SIZE 8

void* sal8_mem_alloc(size_t size, const char* file, uint32_t line) {
    void* ptr = malloc(size);

    if(ptr == NULL)
        sal8_mem_panic();

    SAL8_MEMORY("+ %p                 at line %u in %s", ptr, line, file);

    return ptr;
}

void* sal8_mem_realloc(void* ptr, size_t size, size_t unitSize, const char* file, uint32_t line) {
    void* newPtr = realloc(ptr, size * unitSize);

    if(newPtr == NULL)
        sal8_mem_panic();

    if(ptr == NULL) {
        SAL8_MEMORY("+ %p                 at line %u in %s", newPtr, line, file);
    } else {
        SAL8_MEMORY("~ %p -> %p     at line %u in %s", ptr, newPtr, line, file);
    }

    return newPtr;
}

void* sal8_mem_expand(void* ptr, size_t* size, size_t unitSize, const char* file, uint32_t line) {
    if(*size < MEM_BLOCK_START_SIZE)
        *size = MEM_BLOCK_START_SIZE;
    else (*size) *= 2;

    return sal8_mem_realloc(ptr, *size, unitSize, file, line);
}

void sal8_mem_free(void* ptr, const char* file, uint32_t line) {
    if(ptr != NULL) {
        free(ptr);
        SAL8_MEMORY("- %p                 at line %u in %s", ptr, line, file);
    }
}

void sal8_mem_panic() {
    exit(SAL8_EXIT_OOM);
}

#undef MEM_BLOCK_START_SIZE