#ifndef SAL8_LIB_MEM_INDEX_H_GUARD
#define SAL8_LIB_MEM_INDEX_H_GUARD

#include <stddef.h>

/// Gets the line and column from an index. Counts LF, ignores CR.
void sal8_lib_mem_lncol(const char* source, size_t index, size_t* ln, size_t* col);

#endif