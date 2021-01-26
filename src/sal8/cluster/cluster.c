#include "cluster.h"
#include "bytecode.h"
#include "../mem/mem.h"

#include "string.h"

void sal8_cluster_init(SAL8Cluster* cl) {
    cl->data = NULL;
    cl->size = 0;
    cl->cap = 0;
}

void sal8_cluster_write(SAL8Cluster* cl, uint8_t* src, uint32_t count) {
    while(cl->cap < cl->size + count)
        cl->data = SAL8_MEM_EXPAND(cl->data, &cl->cap, sizeof(uint8_t));

    memcpy(cl->data + cl->size, src, count);
    cl->size += count;
}

void sal8_cluster_delete(SAL8Cluster* cl) {
    SAL8_FREE(cl->data);
    sal8_cluster_init(cl);
}

uint32_t sal8_cluster_instruction_count(SAL8Cluster* cl) {
    return cl->size / SAL8_BYTECODE_FORMAT;
}