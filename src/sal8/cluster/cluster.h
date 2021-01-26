#ifndef SAL8_CLUSTER_H_GUARD
#define SAL8_CLUSTER_H_GUARD

#include "../api.h"
#include <stdint.h>

typedef struct {
    uint8_t* data;
    uint32_t size;
    uint32_t cap;
} SAL8Cluster;

SAL8_API void     sal8_cluster_init(SAL8Cluster* cl);
SAL8_API void     sal8_cluster_write(SAL8Cluster* cl, uint8_t* src, uint32_t count);
SAL8_API void     sal8_cluster_delete(SAL8Cluster* cl);
SAL8_API uint32_t sal8_cluster_instruction_count(SAL8Cluster* cl);

#endif
