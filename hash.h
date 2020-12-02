#pragma once

#include "common.h"
#include "vector.h"

/* Reversible 32- and 64-bit integer hash functions taken from https://stackoverflow.com/a/12996028 */

u32 hash32 (u32 x) {
    x = ((x >> 16) ^ x) * UINT32_C(0x045d9f3b);
    x = ((x >> 16) ^ x) * UINT32_C(0x045d9f3b);
    x = ((x >> 16) ^ x);
    return x;
}

u32 unhash32 (u32 x) {
    x = ((x >> 16) ^ x) * UINT32_C(0x119de1f3);
    x = ((x >> 16) ^ x) * UINT32_C(0x119de1f3);
    x = ((x >> 16) ^ x);
    return x;
}

u64 hash64 (u64 x) {
    x = (x ^ (x >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
    x = (x ^ (x >> 27)) * UINT64_C(0x94d049bb133111eb);
    x = (x ^ (x >> 31));
    return x;
}

u64 unhash64 (u64 x) {
    x = (x ^ (x >> 31) ^ (x >> 62)) * UINT64_C(0x319642b2d24d8ec3);
    x = (x ^ (x >> 27) ^ (x >> 54)) * UINT64_C(0x96de1b173f119089);
    x = (x ^ (x >> 30) ^ (x >> 60));
    return x;
}

typedef struct HashSet {
    size_t n_buckets;
    struct Vector * buckets;
} HashSet_t;

void hashset_init (struct HashSet * h, size_t n_buckets) {
    h->n_buckets = n_buckets;
    h->buckets = malloc(n_buckets * sizeof(struct Vector));
    for (size_t i = 0; i < n_buckets; ++i) {
        vector_init(&(h->buckets[i]), 0);
    }
}

void hashset_clear (struct HashSet * h) {
    for (size_t i = 0; i < h->n_buckets; ++i) {
        vector_clear(&(h->buckets[i]));
    }
}

void hashset_add (struct HashSet * h, s32 x) {
    u32 bucket_index = hash32((u32)x) % h->n_buckets;
    struct Vector * b = &(h->buckets[bucket_index]);
    ptrdiff_t i = vector_find_sorted(b, x);
    if (i < 0) {
        vector_insert_sorted(b, x);
    }
}

void hashset_remove (struct HashSet * h, s32 x) {
    u32 bucket_index = hash32((u32)x) % h->n_buckets;
    struct Vector * b = &(h->buckets[bucket_index]);
    ptrdiff_t i = vector_find_sorted(b, x);
    if (i >= 0) {
        vector_erase(b, i);
    }
}

_Bool hashset_contains (struct HashSet * h, s32 x) {
    u32 bucket_index = hash32((u32)x) % h->n_buckets;
    struct Vector * b = &(h->buckets[bucket_index]);
    ptrdiff_t i = vector_find_sorted(b, x);
    return (i>=0);
}

typedef struct HashMap {
    size_t n_buckets;
    struct Vector * buckets;
} HashSet_t;

void hashmap_init (struct HashMap * h, size_t n_buckets) {
    h->n_buckets = n_buckets;
    h->buckets = malloc(n_buckets * sizeof(struct Vector));
    for (size_t i = 0; i < n_buckets; ++i) {
        vector_init(&(h->buckets[i]), 0);
    }
}

void hashmap_clear (struct HashMap * h) {
    for (size_t i = 0; i < h->n_buckets; ++i) {
        vector_clear(&(h->buckets[i]));
    }
}

void hashmap_add (struct HashMap * h, s32 x) {
    u32 bucket_index = hash32((u32)x) % h->n_buckets;
    struct Vector * b = &(h->buckets[bucket_index]);
    ptrdiff_t i = vector_find_sorted(b, x);
    if (i < 0) {
        vector_insert_sorted(b, x);
    }
}

void hashmap_remove (struct HashMap * h, s32 x) {
    u32 bucket_index = hash32((u32)x) % h->n_buckets;
    struct Vector * b = &(h->buckets[bucket_index]);
    ptrdiff_t i = vector_find_sorted(b, x);
    if (i >= 0) {
        vector_erase(b, i);
    }
}

_Bool hashmap_contains (struct HashMap * h, s32 x) {
    u32 bucket_index = hash32((u32)x) % h->n_buckets;
    struct Vector * b = &(h->buckets[bucket_index]);
    ptrdiff_t i = vector_find_sorted(b, x);
    return (i>=0);
}
