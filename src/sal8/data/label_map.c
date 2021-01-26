#include "label_map.h"
#include "../mem/mem.h"
#include "../lib/charlib.h"

#include <string.h>

#define MAP_MAX_LOAD 0.75
#define MAP_START_SIZE 8
#define MAP_TOMBSTONE UINT8_MAX

#include <stdio.h>

static SAL8LabelEntry* sal8_label_map_find_bucket(SAL8LabelEntry* entries, int capacity, const char* key, size_t keySize) {
    uint32_t index = sal8_label_map_hash(key, keySize) & (capacity - 1);
    SAL8LabelEntry* tombstone = NULL;

    while(1) {
        SAL8LabelEntry* entry = &entries[index];

        if(entry->key == NULL) {
            if(entry->value != MAP_TOMBSTONE) {
                return tombstone != NULL ? tombstone : entry;
            } else {
                if(tombstone == NULL)
                    tombstone = entry;
            }
        } else if(sal8_lib_charlib_strmcmp(entry->key, key, keySize)) {
            return entry;
        }

        index = (index + 1) & (capacity - 1);
    }
}

static void sal8_label_map_adjust_capacity(SAL8LabelMap* map) {
    if(map->count + 1 > map->capacity * MAP_MAX_LOAD) {
        uint32_t capacity = map->capacity < MAP_START_SIZE ? MAP_START_SIZE : 2 * map->capacity;
        SAL8LabelEntry* entries = SAL8_MEM_ALLOC(capacity * sizeof(SAL8LabelEntry));

        for(uint32_t i = 0; i < capacity; ++i) {
            entries[i].key = NULL;
            entries[i].value = 0;
        }

        map->count = 0;
        for(uint32_t i = 0; i < map->capacity; ++i) {
            SAL8LabelEntry* entry = &map->entries[i];
            if (entry->key == NULL)
                continue;

            SAL8LabelEntry* dest = sal8_label_map_find_bucket(entries, capacity, entry->key, strlen(entry->key));
            dest->key = entry->key;
            dest->value = entry->value;

            ++map->count;
        }

        SAL8_MEM_FREE(map->entries);
        map->entries = entries;
        map->capacity = capacity;
    }
}

void sal8_label_map_init(SAL8LabelMap* map) {
    map->count = 0;
    map->capacity = 0;
    map->entries = NULL;
}

void sal8_label_map_delete(SAL8LabelMap* map) {
    for(size_t i = 0; i < map->capacity; ++i) {
        if(map->entries[i].key == NULL)
            continue;

        SAL8_MEM_FREE(map->entries[i].key);
    }

    SAL8_MEM_FREE(map->entries);
    sal8_label_map_init(map);
}

uint32_t sal8_label_map_hash(const char* chars, size_t size) {
    uint32_t hash = 2166136261u;

    const char* ptr = chars;

    while(*ptr != '\0' && size > 0) {
        hash ^= *ptr++;
        hash *= 16777619;
        --size;
    }

    return hash;
}

bool sal8_label_map_get(SAL8LabelMap* map, const char* key, uint8_t* value) {
    return sal8_label_map_nget(map, key, strlen(key), value);
}

bool sal8_label_map_nget(SAL8LabelMap* map, const char* key, size_t keySize, uint8_t* value) {
    if(map->count == 0)
        return false;

    SAL8LabelEntry* entry = sal8_label_map_find_bucket(map->entries, map->capacity, key, keySize);

    if(entry->key == NULL)
        return false;

    *value = entry->value;
    return true;
}

bool sal8_label_map_set(SAL8LabelMap* map, const char* key, uint8_t value) {
    return sal8_label_map_nset(map, key, strlen(key), value);
}

bool sal8_label_map_nset(SAL8LabelMap* map, const char* key, size_t keySize, uint8_t value) {
    sal8_label_map_adjust_capacity(map);

    SAL8LabelEntry* entry = sal8_label_map_find_bucket(map->entries, map->capacity, key, keySize);

    if(entry->key != NULL)
        return false;

    if(entry->value != MAP_TOMBSTONE) // Not a tombstone.
        ++map->count;

    entry->key = sal8_lib_charlib_strndup(key, keySize);
    entry->value = value;

    return true;
}

bool sal8_label_map_erase(SAL8LabelMap* map, const char* key) {
    if(map->count == 0)
        return false;

    SAL8LabelEntry* entry = sal8_label_map_find_bucket(map->entries, map->capacity, key, strlen(key));
    if(entry->key == NULL)
        return false;

    entry->key = NULL;
    entry->value = MAP_TOMBSTONE;

    return true;
}

void sal8_label_map_copy(SAL8LabelMap* map, SAL8LabelMap* from) {
    for(uint32_t i = 0; i < from->capacity; ++i) {
        SAL8LabelEntry* entry = &from->entries[i];
        if(entry->key != NULL)
            sal8_label_map_set(map, entry->key, entry->value);
    }
}