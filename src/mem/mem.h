#ifndef SAL8_MEM_H_GUARD
#define SAL8_MEM_H_GUARD

#include <stdint.h>
#include <stddef.h>

#define SAL8_EXIT_OOM 137

#define SAL8_MEM_ALLOC(size) sal8_mem_alloc(size, __FILE__, __LINE__)
#define SAL8_MEM_REALLOC(ptr, size, unitSize) sal8_mem_realloc(ptr, size, unitSize, __FILE__, __LINE__)
#define SAL8_MEM_EXPAND(ptr, size, unitSize) sal8_mem_expand(ptr, size, unitSize, __FILE__, __LINE__)
#define SAL8_FREE(ptr) sal8_mem_free(ptr, __FILE__, __LINE__)

void* sal8_mem_alloc(size_t size, const char* file, uint32_t line);
void* sal8_mem_realloc(void* ptr, size_t size, size_t unitSize, const char* file, uint32_t line);
void* sal8_mem_expand(void* ptr, size_t* size, size_t unitSize, const char* file, uint32_t line);
void  sal8_mem_free(void* ptr, const char* file, uint32_t line);
void  sal8_mem_panic();

#endif
