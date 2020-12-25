#pragma once

#include "hash.h"

typedef struct Map {
    size_t capacity;
    size_t count;
    intptr_t * keys;
    intptr_t * values;
} Map_t;

void map_init (struct Map * map, size_t capacity) {
    ASSERT(map);
    map->capacity = capacity;
    map->count = 0;
    if (capacity) {
        map->keys = calloc(capacity, sizeof(intptr_t));
        map->values = calloc(capacity, sizeof(intptr_t));
    } else {
        map->keys = NULL;
        map->values = NULL;
    }
}

void map_free (struct Map * map) {
    map->capacity = 0;
    map->count = 0;
    free(map->keys);
    map->keys = NULL;
    free(map->values);
    map->values = NULL;
}

void map_resize (struct Map * map, size_t capacity) {
    if (capacity) {
        map->capacity = capacity;
        map->count = MIN(map->count, capacity);
        map->keys = realloc(map->keys, capacity*sizeof(intptr_t));
        map->values = realloc(map->values, capacity*sizeof(intptr_t));
    } else {
        map_free(map);
    }
}

void map_copy (struct Map * dest, struct Map const * src) {
    ASSERT(dest);
    ASSERT(src);
    map_init(dest, src->capacity);
    memcpy(dest->keys, src->keys, src->count * sizeof(intptr_t));
    memcpy(dest->values, src->values, src->count * sizeof(intptr_t));
    dest->count = src->count;
}

void map_move (struct Map * dest, struct Map * src) {
    ASSERT(dest);
    ASSERT(src);
    dest->capacity = src->capacity;
    dest->count = src->count;
    dest->keys = src->keys;
    dest->values = src->values;
    src->capacity = 0;
    src->count = 0;
    src->keys = NULL;
    src->values = NULL;
}

void map_grow (struct Map * map) {
    ASSERT(map);
    size_t capacity = map->capacity;
    capacity = capacity + MAX(capacity / 2, 1);
    map_resize(map, capacity);
}

void map_ensure_capacity (struct Map * map, size_t capacity) {
    ASSERT(map);
    while (map->capacity < capacity) {
        map_grow(map);
    }
}

void map_print (struct Map const * map) {
    printf("Map with %zu elements: {\n", map->count);
    for (size_t i = 0; i < map->count; ++i) {
        printf("%8zi => %zi\n", map->keys[i], map->values[i]);
    }
    printf("}\n");
}

ptrdiff_t map_find (struct Map const * map, intptr_t key) {
    if (map->count) {
        if (key >= map->keys[0] && key <= map->keys[map->count-1]) {
            intptr_t const * pl = &(map->keys[0]);
            intptr_t const * pu = &(map->keys[map->count]);
            while ((pu-pl) > 1) {
                intptr_t const * pm = pl + (pu-pl)/2;
                intptr_t km = *pm;
                if (key >= km) {
                    pl = pm;
                } else if (key < km) {
                    pu = pm;
                }
            }
            if (*pl == key) {
                return (pl-&(map->keys[0]));
            }
        }
    }
    return -1;
}

_Bool map_contains (struct Map const * map, intptr_t key) {
    return (map_find(map, key) >= 0);
}

intptr_t map_get (struct Map const * map, intptr_t key) {
    ptrdiff_t ind = map_find(map, key);
    if (ind >= 0) {
        return map->values[ind];
    }
    ASSERT(0);
    return 0;
}

void map_insert (struct Map * map, size_t index, intptr_t key, intptr_t value) {
    ASSERT(index <= map->count);
    map_ensure_capacity(map, map->count+1);
    if (index != map->count) {
        memmove(&(map->keys[index+1]), &(map->keys[index]), (map->count - index)*sizeof(intptr_t));
        memmove(&(map->values[index+1]), &(map->values[index]), (map->count - index)*sizeof(intptr_t));
    }
    map->keys[index] = key;
    map->values[index] = value;
    map->count += 1;
}

void map_set (struct Map * map, intptr_t key, intptr_t value) {
    if (map->count) {
        if (key < map->keys[0]) {
            map_insert(map, 0, key, value);
        } else if (key > map->keys[map->count-1]) {
            map_insert(map, map->count, key, value);
        } else {
            intptr_t const * pl = &(map->keys[0]);
            intptr_t const * pu = &(map->keys[map->count]);
            while ((pu-pl) > 1) {
                intptr_t const * pm = pl + (pu-pl)/2;
                intptr_t km = *pm;
                if (key >= km) {
                    pl = pm;
                } else {
                    pu = pm;
                }
            }
            ptrdiff_t ind = pl - &(map->keys[0]);
            if (*pl == key) {
                map->values[ind] = value;
            } else {
                map_insert(map, ind+1, key, value);
            }
        }
    } else {
        map_insert(map, 0, key, value);
    }
}

// TODO: map_remove

typedef struct HashMap {
    size_t n_bins;
    struct Map * bins;
} HashMap_t;

void hashmap_init (struct HashMap * map, size_t n_bins, size_t capacity) {
    ASSERT(map);
    ASSERT(n_bins);
    map->n_bins = n_bins;
    map->bins = calloc(n_bins, sizeof(struct Map));
    for (size_t i = 0; i < n_bins; ++i) {
        map_init(&(map->bins[i]), capacity);
    }
}

void hashmap_free (struct HashMap * map) {
    for (size_t i = 0; i < map->n_bins; ++i) {
        map_free(&(map->bins[i]));
    }
    free(map->bins);
    map->bins = NULL;
    map->n_bins = 0;
}

size_t hashmap_count (struct HashMap const * set) {
    size_t n = 0;
    for (size_t i = 0; i < set->n_bins; ++i) {
        n += set->bins[i].count;
    }
    return n;
}

_Bool hashmap_contains (struct HashMap const * map, intptr_t key) {
    size_t bin_index = HASH(key) % map->n_bins;
    return (map_find(&(map->bins[bin_index]), key) != -1);
}

intptr_t hashmap_get (struct HashMap const * map, intptr_t key) {
    size_t bin_index = HASH(key) % map->n_bins;
    return map_get(&(map->bins[bin_index]), key);
}

void hashmap_set (struct HashMap * map, intptr_t key, intptr_t value) {
    size_t bin_index = HASH(key) % map->n_bins;
    map_set(&(map->bins[bin_index]), key, value);
}
