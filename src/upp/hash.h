/*
 * UPP String-String Hashmap
 */

#ifndef UPP_HASH_H
#define UPP_HASH_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* TCC compatibility */
#ifdef TCC_TARGET_X86_64
#include "../tcc.h"
#define UPP_HASH_MALLOC(s) tcc_malloc(s)
#define UPP_HASH_FREE(p) tcc_free(p)
#define UPP_HASH_STRDUP(s) tcc_strdup(s)
#define UPP_HASH_CALLOC(n,s) tcc_mallocz((n)*(s))
#else
#define UPP_HASH_MALLOC(s) malloc(s)
#define UPP_HASH_FREE(p) free(p)
#define UPP_HASH_STRDUP(s) strdup(s)
#define UPP_HASH_CALLOC(n,s) calloc(n,s)
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define UPP_HASH_DEFAULT_CAPACITY 16

#define UPP_HASH_LOAD_FACTOR 0.75

typedef struct upp_hash_entry {
    char *key;
    char *value;
    struct upp_hash_entry *next;
} upp_hash_entry_t;

typedef struct upp_hash {
    upp_hash_entry_t **buckets;
    size_t capacity;
    size_t size;
    size_t threshold;
} upp_hash_t;

typedef struct upp_hash_iterator {
    upp_hash_t *hash;
    size_t bucket_index;
    upp_hash_entry_t *current_entry;
} upp_hash_iterator_t;

static inline uint32_t upp_hash_function(const char *str) {
    uint32_t hash = 2166136261U;
    while (*str) {
        hash ^= (uint32_t)*str++;
        hash *= 16777619U;
    }
    return hash;
}

static inline char *upp_strdup(const char *str) {
    char *dup;
    size_t len;
    
    if (!str) return NULL;
    len = strlen(str) + 1;
    dup = (char*)UPP_HASH_MALLOC(len);
    if (dup) {
        memcpy(dup, str, len);
    }
    return dup;
}

static inline upp_hash_t *upp_hash_create(void) {
    upp_hash_t *hash;
    
    hash = (upp_hash_t*)UPP_HASH_MALLOC(sizeof(upp_hash_t));
    if (!hash) return NULL;
    
    hash->capacity = UPP_HASH_DEFAULT_CAPACITY;
    hash->size = 0;
    hash->threshold = (size_t)(hash->capacity * UPP_HASH_LOAD_FACTOR);
    
    hash->buckets = (upp_hash_entry_t**)UPP_HASH_CALLOC(hash->capacity, sizeof(upp_hash_entry_t*));
    if (!hash->buckets) {
        UPP_HASH_FREE(hash);
        return NULL;
    }
    
    return hash;
}

static inline upp_hash_t *upp_hash_create_with_capacity(size_t capacity) {
    if (capacity < 1) capacity = UPP_HASH_DEFAULT_CAPACITY;
    
    upp_hash_t *hash = (upp_hash_t*)UPP_HASH_MALLOC(sizeof(upp_hash_t));
    if (!hash) return NULL;
    
    hash->capacity = capacity;
    hash->size = 0;
    hash->threshold = (size_t)(hash->capacity * UPP_HASH_LOAD_FACTOR);
    
    hash->buckets = (upp_hash_entry_t**)UPP_HASH_CALLOC(hash->capacity, sizeof(upp_hash_entry_t*));
    if (!hash->buckets) {
        UPP_HASH_FREE(hash);
        return NULL;
    }
    
    return hash;
}

static inline void upp_hash_destroy(upp_hash_t *hash) {
    if (!hash) return;
    
    for (size_t i = 0; i < hash->capacity; i++) {
        upp_hash_entry_t *entry = hash->buckets[i];
        while (entry) {
            upp_hash_entry_t *next = entry->next;
            UPP_HASH_FREE(entry->key);
            UPP_HASH_FREE(entry->value);
            UPP_HASH_FREE(entry);
            entry = next;
        }
    }
    
    UPP_HASH_FREE(hash->buckets);
    UPP_HASH_FREE(hash);
}

static inline bool upp_hash_resize(upp_hash_t *hash) {
    size_t old_capacity = hash->capacity;
    upp_hash_entry_t **old_buckets = hash->buckets;
    
    hash->capacity *= 2;
    hash->threshold = (size_t)(hash->capacity * UPP_HASH_LOAD_FACTOR);
    hash->buckets = (upp_hash_entry_t**)UPP_HASH_CALLOC(hash->capacity, sizeof(upp_hash_entry_t*));
    
    if (!hash->buckets) {
        /* Restore old state on failure */
        hash->capacity = old_capacity;
        hash->threshold = (size_t)(hash->capacity * UPP_HASH_LOAD_FACTOR);
        hash->buckets = old_buckets;
        return false;
    }
    
    hash->size = 0;
    
    for (size_t i = 0; i < old_capacity; i++) {
        upp_hash_entry_t *entry = old_buckets[i];
        while (entry) {
            upp_hash_entry_t *next = entry->next;
            
            uint32_t hash_value = upp_hash_function(entry->key);
            size_t index = hash_value % hash->capacity;
            
            entry->next = hash->buckets[index];
            hash->buckets[index] = entry;
            hash->size++;
            
            entry = next;
        }
    }
    
    UPP_HASH_FREE(old_buckets);
    return true;
}

static inline bool upp_hash_put(upp_hash_t *hash, const char *key, const char *value) {
    if (!hash || !key) return false;
    
    uint32_t hash_value = upp_hash_function(key);
    size_t index = hash_value % hash->capacity;
    
    upp_hash_entry_t *entry = hash->buckets[index];
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            UPP_HASH_FREE(entry->value);
            entry->value = upp_strdup(value);
            return entry->value != NULL;
        }
        entry = entry->next;
    }
    
    entry = (upp_hash_entry_t*)UPP_HASH_MALLOC(sizeof(upp_hash_entry_t));
    if (!entry) return false;
    
    entry->key = upp_strdup(key);
    entry->value = upp_strdup(value);
    if (!entry->key || !entry->value) {
        UPP_HASH_FREE(entry->key);
        UPP_HASH_FREE(entry->value);
        UPP_HASH_FREE(entry);
        return false;
    }
    
    entry->next = hash->buckets[index];
    hash->buckets[index] = entry;
    hash->size++;
    
    if (hash->size >= hash->threshold) {
        upp_hash_resize(hash);
    }
    
    return true;
}

static inline const char *upp_hash_get(const upp_hash_t *hash, const char *key) {
    if (!hash || !key) return NULL;
    
    uint32_t hash_value = upp_hash_function(key);
    size_t index = hash_value % hash->capacity;
    
    upp_hash_entry_t *entry = hash->buckets[index];
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }
    
    return NULL;
}

static inline bool upp_hash_contains(const upp_hash_t *hash, const char *key) {
    return upp_hash_get(hash, key) != NULL;
}

static inline bool upp_hash_remove(upp_hash_t *hash, const char *key) {
    if (!hash || !key) return false;
    
    uint32_t hash_value = upp_hash_function(key);
    size_t index = hash_value % hash->capacity;
    
    upp_hash_entry_t *entry = hash->buckets[index];
    upp_hash_entry_t *prev = NULL;
    
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            if (prev) {
                prev->next = entry->next;
            } else {
                hash->buckets[index] = entry->next;
            }
            
            UPP_HASH_FREE(entry->key);
            UPP_HASH_FREE(entry->value);
            UPP_HASH_FREE(entry);
            hash->size--;
            return true;
        }
        prev = entry;
        entry = entry->next;
    }
    
    return false;
}

static inline size_t upp_hash_size(const upp_hash_t *hash) {
    return hash ? hash->size : 0;
}

static inline bool upp_hash_is_empty(const upp_hash_t *hash) {
    return upp_hash_size(hash) == 0;
}

static inline void upp_hash_clear(upp_hash_t *hash) {
    if (!hash) return;
    
    for (size_t i = 0; i < hash->capacity; i++) {
        upp_hash_entry_t *entry = hash->buckets[i];
        while (entry) {
            upp_hash_entry_t *next = entry->next;
            UPP_HASH_FREE(entry->key);
            UPP_HASH_FREE(entry->value);
            UPP_HASH_FREE(entry);
            entry = next;
        }
        hash->buckets[i] = NULL;
    }
    
    hash->size = 0;
}

static inline upp_hash_iterator_t upp_hash_iterator_init(upp_hash_t *hash) {
    upp_hash_iterator_t iter = {0};
    if (!hash) return iter;
    
    iter.hash = hash;
    iter.bucket_index = 0;
    iter.current_entry = NULL;
    
    for (size_t i = 0; i < hash->capacity; i++) {
        if (hash->buckets[i]) {
            iter.bucket_index = i;
            iter.current_entry = hash->buckets[i];
            break;
        }
    }
    
    return iter;
}

static inline bool upp_hash_iterator_has_next(const upp_hash_iterator_t *iter) {
    return iter && iter->current_entry != NULL;
}

static inline upp_hash_entry_t *upp_hash_iterator_next(upp_hash_iterator_t *iter) {
    if (!iter || !iter->current_entry) return NULL;
    
    upp_hash_entry_t *entry = iter->current_entry;
    
    if (iter->current_entry->next) {
        iter->current_entry = iter->current_entry->next;
    } else {
        iter->current_entry = NULL;
        for (size_t i = iter->bucket_index + 1; i < iter->hash->capacity; i++) {
            if (iter->hash->buckets[i]) {
                iter->bucket_index = i;
                iter->current_entry = iter->hash->buckets[i];
                break;
            }
        }
    }
    
    return entry;
}

static inline void upp_hash_print(const upp_hash_t *hash) {
    if (!hash) {
        printf("Hash: NULL\n");
        return;
    }
    
    printf("Hash: size=%zu, capacity=%zu\n", hash->size, hash->capacity);
    
    upp_hash_iterator_t iter = upp_hash_iterator_init((upp_hash_t*)hash);
    while (upp_hash_iterator_has_next(&iter)) {
        upp_hash_entry_t *entry = upp_hash_iterator_next(&iter);
        printf("  \"%s\" -> \"%s\"\n", entry->key, entry->value);
    }
}

#ifdef __cplusplus
}
#endif

#endif
