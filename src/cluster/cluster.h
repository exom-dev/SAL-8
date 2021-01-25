#ifndef SAL8_CLUSTER_H_GUARD
#define SAL8_CLUSTER_H_GUARD

#include <stdint.h>

typedef struct {
    uint8_t* data;
    uint32_t size;
    uint32_t cap;
} SAL8Cluster;

void sal8_cluster_init(SAL8Cluster* cl);
void sal8_cluster_write(SAL8Cluster* cl, uint8_t* src, uint32_t count);
void sal8_cluster_delete(SAL8Cluster* cl);

#endif
