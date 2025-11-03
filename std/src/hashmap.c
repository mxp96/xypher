#include "../include/xystd.h"
#include <stdlib.h>
#include <string.h>

#define HASHMAP_INITIAL_CAPACITY 16
#define HASHMAP_LOAD_FACTOR 0.75
#define HASHMAP_GROWTH_FACTOR 2

typedef struct xy_hashmap_entry {
    char* key;
    void* value;
    struct xy_hashmap_entry* next;
} xy_hashmap_entry;

struct xy_hashmap {
    xy_hashmap_entry** buckets;
    unsigned long long capacity;
    unsigned long long size;
    unsigned long long threshold;
};

static unsigned long long hash_string(const char* key, unsigned long long capacity) {
    unsigned long long hash = 5381;
    int c;
    
    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c;
    }
    
    return hash % capacity;
}

static xy_hashmap_entry* create_entry(const char* key, void* value) {
    xy_hashmap_entry* entry = (xy_hashmap_entry*)malloc(sizeof(xy_hashmap_entry));
    if (!entry) return NULL;
    
    entry->key = (char*)malloc(strlen(key) + 1);
    if (!entry->key) {
        free(entry);
        return NULL;
    }
    
    strcpy(entry->key, key);
    entry->value = value;
    entry->next = NULL;
    
    return entry;
}

static void destroy_entry(xy_hashmap_entry* entry) {
    if (!entry) return;
    free(entry->key);
    free(entry);
}

static int resize_hashmap(xy_hashmap* map, unsigned long long new_capacity) {
    xy_hashmap_entry** old_buckets = map->buckets;
    unsigned long long old_capacity = map->capacity;
    
    xy_hashmap_entry** new_buckets = (xy_hashmap_entry**)calloc(new_capacity, sizeof(xy_hashmap_entry*));
    if (!new_buckets) return 0;
    
    map->buckets = new_buckets;
    map->capacity = new_capacity;
    map->threshold = (unsigned long long)(new_capacity * HASHMAP_LOAD_FACTOR);
    map->size = 0;
    
    for (unsigned long long i = 0; i < old_capacity; i++) {
        xy_hashmap_entry* entry = old_buckets[i];
        while (entry) {
            xy_hashmap_entry* next = entry->next;
            
            unsigned long long index = hash_string(entry->key, new_capacity);
            entry->next = new_buckets[index];
            new_buckets[index] = entry;
            map->size++;
            
            entry = next;
        }
    }
    
    free(old_buckets);
    return 1;
}

xy_hashmap* xy_hashmap_create(unsigned long long capacity) {
    if (capacity == 0) {
        capacity = HASHMAP_INITIAL_CAPACITY;
    }
    
    xy_hashmap* map = (xy_hashmap*)malloc(sizeof(xy_hashmap));
    if (!map) return NULL;
    
    map->buckets = (xy_hashmap_entry**)calloc(capacity, sizeof(xy_hashmap_entry*));
    if (!map->buckets) {
        free(map);
        return NULL;
    }
    
    map->capacity = capacity;
    map->size = 0;
    map->threshold = (unsigned long long)(capacity * HASHMAP_LOAD_FACTOR);
    
    return map;
}

void xy_hashmap_destroy(xy_hashmap* map) {
    if (!map) return;
    
    for (unsigned long long i = 0; i < map->capacity; i++) {
        xy_hashmap_entry* entry = map->buckets[i];
        while (entry) {
            xy_hashmap_entry* next = entry->next;
            destroy_entry(entry);
            entry = next;
        }
    }
    
    free(map->buckets);
    free(map);
}

int xy_hashmap_insert(xy_hashmap* map, const char* key, void* value) {
    if (!map || !key) return 0;
    
    if (map->size >= map->threshold) {
        if (!resize_hashmap(map, map->capacity * HASHMAP_GROWTH_FACTOR)) {
            return 0;
        }
    }
    
    unsigned long long index = hash_string(key, map->capacity);
    xy_hashmap_entry* entry = map->buckets[index];
    
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            entry->value = value;
            return 1;
        }
        entry = entry->next;
    }
    
    xy_hashmap_entry* new_entry = create_entry(key, value);
    if (!new_entry) return 0;
    
    new_entry->next = map->buckets[index];
    map->buckets[index] = new_entry;
    map->size++;
    
    return 1;
}

void* xy_hashmap_get(xy_hashmap* map, const char* key) {
    if (!map || !key) return NULL;
    
    unsigned long long index = hash_string(key, map->capacity);
    xy_hashmap_entry* entry = map->buckets[index];
    
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }
    
    return NULL;
}

int xy_hashmap_remove(xy_hashmap* map, const char* key) {
    if (!map || !key) return 0;
    
    unsigned long long index = hash_string(key, map->capacity);
    xy_hashmap_entry* entry = map->buckets[index];
    xy_hashmap_entry* prev = NULL;
    
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            if (prev) {
                prev->next = entry->next;
            } else {
                map->buckets[index] = entry->next;
            }
            
            destroy_entry(entry);
            map->size--;
            return 1;
        }
        
        prev = entry;
        entry = entry->next;
    }
    
    return 0;
}

int xy_hashmap_contains(xy_hashmap* map, const char* key) {
    if (!map || !key) return 0;
    
    unsigned long long index = hash_string(key, map->capacity);
    xy_hashmap_entry* entry = map->buckets[index];
    
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            return 1;
        }
        entry = entry->next;
    }
    
    return 0;
}

unsigned long long xy_hashmap_size(xy_hashmap* map) {
    return map ? map->size : 0;
}

void xy_hashmap_clear(xy_hashmap* map) {
    if (!map) return;
    
    for (unsigned long long i = 0; i < map->capacity; i++) {
        xy_hashmap_entry* entry = map->buckets[i];
        while (entry) {
            xy_hashmap_entry* next = entry->next;
            destroy_entry(entry);
            entry = next;
        }
        map->buckets[i] = NULL;
    }
    
    map->size = 0;
}

const char** xy_hashmap_keys(xy_hashmap* map, unsigned long long* count) {
    if (!map || !count) {
        if (count) *count = 0;
        return NULL;
    }
    
    if (map->size == 0) {
        *count = 0;
        return NULL;
    }
    
    const char** keys = (const char**)malloc(map->size * sizeof(char*));
    if (!keys) {
        *count = 0;
        return NULL;
    }
    
    unsigned long long key_index = 0;
    for (unsigned long long i = 0; i < map->capacity; i++) {
        xy_hashmap_entry* entry = map->buckets[i];
        while (entry) {
            keys[key_index++] = entry->key;
            entry = entry->next;
        }
    }
    
    *count = map->size;
    return keys;
}

void xy_hashmap_free_keys(const char** keys) {
    if (keys) {
        free((void*)keys);
    }
}

