#pragma once

typedef struct IntMap {
    size_t capacity;
    size_t count;
    intptr_t * keys;
    intptr_t * values;
} IntMap_t;


void intmap_init (struct IntMap * map, size_t capacity) {
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

void intmap_free (struct IntMap * map) {
    map->capacity = 0;
    map->count = 0;
    free(map->keys);
    map->keys = NULL;
    free(map->values);
    map->values = NULL;
}

void intmap_resize (struct IntMap * map, size_t capacity) {
    if (capacity) {
        map->capacity = capacity;
        map->count = MIN(map->count, capacity);
        map->keys = realloc(map->keys, capacity*sizeof(intptr_t));
        map->values = realloc(map->values, capacity*sizeof(intptr_t));
    } else {
        intmap_free(map);
    }
}

void intmap_copy (struct IntMap * dest, struct IntMap const * src) {
    ASSERT(dest);
    ASSERT(src);
    intmap_init(dest, src->capacity);
    memcpy(dest->keys, src->keys, src->count * sizeof(intptr_t));
    memcpy(dest->values, src->values, src->count * sizeof(intptr_t));
    dest->count = src->count;
}

void intmap_move (struct IntMap * dest, struct IntMap * src) {
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

void intmap_grow (struct IntMap * map) {
    ASSERT(map);
    size_t capacity = map->capacity;
    capacity = capacity + MAX(capacity / 2, 1);
    intmap_resize(map, capacity);
}

void intmap_ensure_capacity (struct IntMap * map, size_t capacity) {
    ASSERT(map);
    while (map->capacity < capacity) {
        intmap_grow(map);
    }
}

void intmap_print (struct IntMap const * map) {
    printf("IntMap with %zu elements: {\n", map->count);
    for (size_t i = 0; i < map->count; ++i) {
        printf("%8zi -> %zi\n", map->keys[i], map->values[i]);
    }
    printf("}\n");
}

ptrdiff_t intmap_find (struct IntMap const * map, intptr_t key) {
    if (map->count) {
        if (key >= map->keys[0] && key <= map->keys[map->count-1]) {
            intptr_t const * pl = &(map->keys[0]);
            intptr_t const * pu = &(map->keys[map->count]);
            while ((pu-pl) > 16) {
                intptr_t const * pm = pl + (pu-pl)/2;
                intptr_t km = *pm;
                if (key < km) {
                    pu = pm;
                } else if (key > km) {
                    pl = pm+1;
                } else {
                    return (pm-&(map->keys[0]));
                }
            }
            while (*pl < key) {
                ++pl;
            }
            if (*pl == key) {
                return (pl-&(map->keys[0]));
            }
        }
    }
    return -1;
}

intptr_t intmap_get (struct IntMap const * map, intptr_t key) {
    ptrdiff_t ind = intmap_find(map, key);
    if (ind >= 0) {
        return map->values[ind];
    }
    return 0;
}

void intmap_insert (struct IntMap * map, size_t index, intptr_t key, intptr_t value) {
    ASSERT(index <= map->count);
    intmap_ensure_capacity(map, map->count+1);
    if (index != map->count) {
        memmove(&(map->keys[index+1]), &(map->keys[index]), (map->count - index)*sizeof(intptr_t));
        memmove(&(map->values[index+1]), &(map->values[index]), (map->count - index)*sizeof(intptr_t));
    }
    map->keys[index] = key;
    map->values[index] = value;
    map->count += 1;
}

void intmap_set (struct IntMap * map, intptr_t key, intptr_t value) {
    if (map->count) {
        ptrdiff_t ind = 0;
        if (key < map->keys[0]) {
            ind = 0;
        } else if (key > map->keys[map->count-1]) {
            ind = map->count;
        } else {
            intptr_t const * pl = &(map->keys[0]);
            intptr_t const * pu = &(map->keys[map->count-1]);
            while ((pu-pl) > 1) {
                intptr_t const * pm = pl + (pu-pl)/2;
                intptr_t km = *pm;
                if (key > km) {
                    pl = pm;
                } else if (key < km) {
                    pu = pm;
                } else {
                    pl = (pu = pm);
                    break;
                }
            }
            ind = (pu-&(map->keys[0]));
        }
        if (map->keys[ind] == key) {
            map->values[ind] = value;
        } else {
            intmap_insert(map, ind, key, value);
        }
    } else {
        intmap_insert(map, 0, key, value);
    }
}
