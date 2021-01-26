#ifndef SAL8_LABEL_MAP_H_GUARD
#define SAL8_LABEL_MAP_H_GUARD

#include "../api.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct {
    char* key;
    uint8_t value;
} SAL8LabelEntry;

typedef struct {
    uint32_t count;
    uint32_t capacity;
    SAL8LabelEntry* entries;
} SAL8LabelMap;

SAL8_API void sal8_label_map_init(SAL8LabelMap* map);
SAL8_API void sal8_label_map_delete(SAL8LabelMap* map);

SAL8_API uint32_t sal8_label_map_hash(const char* chars, size_t size);

SAL8_API bool sal8_label_map_get(SAL8LabelMap* map, const char* key, uint8_t* value);
SAL8_API bool sal8_label_map_set(SAL8LabelMap* map, const char* key, uint8_t value);
SAL8_API bool sal8_label_map_nget(SAL8LabelMap* map, const char* key, size_t keySize, uint8_t* value);
SAL8_API bool sal8_label_map_nset(SAL8LabelMap* map, const char* key, size_t keySize, uint8_t value);
SAL8_API bool sal8_label_map_erase(SAL8LabelMap* map, const char* key);
SAL8_API void sal8_label_map_copy(SAL8LabelMap* map, SAL8LabelMap* from);

#endif
