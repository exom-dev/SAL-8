#include "mem_index.h"

void sal8_lib_mem_lncol(const char* source, size_t index, size_t* ln, size_t* col) {
    size_t l = 1;
    size_t c = 1;
    char current;

    const char* limit = source + index;

    while(source < limit) {
        current = *(source++);

        if(current == '\n') {
            ++l;
            c = 1;
        } else if(current != '\r') {
            ++c;
        }
    }

    *ln = l;
    *col = c;
}