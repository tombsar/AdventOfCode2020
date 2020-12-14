#include "common.h"
#include "stringview.h"
#include "vector.h"

typedef struct IntMapElement {
    intptr_t key;
    intptr_t value;
} IntMapElement_t;

typedef struct IntMap {
    size_t capacity;
    size_t count;
    struct IntMapElement * data;
} IntMap_t;


void intmap_init (struct IntMap * map, size_t capacity) {
    ASSERT(map);
    map->capacity = capacity;
    map->count = 0;
    if (capacity) {
        map->data = calloc(capacity, sizeof(struct IntMapElement));
    } else {
        map->data = NULL;
    }
}

void intmap_free (struct IntMap * map) {
    map->capacity = 0;
    map->count = 0;
    free(map->data);
    map->data = NULL;
}

void intmap_resize (struct IntMap * map, size_t capacity) {
    if (capacity) {
        map->capacity = capacity;
        map->count = MIN(map->count, capacity);
        map->data = realloc(map->data, capacity*sizeof(struct IntMapElement));
    } else {
        intmap_free(map);
    }
}

void intmap_copy (struct IntMap * dest, struct IntMap const * src) {
    ASSERT(dest);
    ASSERT(src);
    intmap_init(dest, src->capacity);
    memcpy(dest->data, src->data, src->count * sizeof(struct IntMapElement));
    dest->count = src->count;
}

void intmap_move (struct IntMap * dest, struct IntMap * src) {
    ASSERT(dest);
    ASSERT(src);
    dest->capacity = src->capacity;
    dest->count = src->count;
    dest->data = src->data;
    src->capacity = 0;
    src->count = 0;
    src->data = NULL;
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
        printf("%8zi -> %zi\n", map->data[i].key, map->data[i].value);
    }
    printf("}\n");
}

struct IntMapElement const * intmap_cbegin (struct IntMap const * map) {
    return &(map->data[0]);
}

struct IntMapElement const * intmap_cend (struct IntMap const * map) {
    return &(map->data[map->count]);
}

intptr_t intmap_find (struct IntMap const * map, intptr_t key) {
    if (map->count) {
        if (key < map->data[0].key || key > map->data[map->count-1].key) {
            return -1;
        } else {
            intptr_t il = 0;
            intptr_t iu = map->count;
            while (iu > il) {
                intptr_t im = il + (iu-il)/2;
                intptr_t km = map->data[im].key;
                if (key < km) {
                    iu = im;
                } else if (key > km) {
                    il = im+1;
                } else {
                    return im;
                }
            }
        }
    }
    return -1;
}

intptr_t intmap_get (struct IntMap const * map, intptr_t key) {
    intptr_t id = intmap_find(map, key);
    if (id >= 0) {
        return map->data[id].value;
    }
    return 0;
}

struct IntMapElement * intmap_insert (struct IntMap * map, size_t index, intptr_t key, intptr_t value) {
    ASSERT(index <= map->count);
    intmap_ensure_capacity(map, map->count+1);
    if (index != map->count) {
        memmove(&(map->data[index+1]), &(map->data[index]), (map->count - index)*sizeof(struct IntMapElement));
    }
    map->data[index].key = key;
    map->data[index].value = value;
    map->count += 1;
    return &(map->data[index]);
}

void intmap_set (struct IntMap * map, intptr_t key, intptr_t value) {
    if (map->count) {
        size_t ind = 0;
        if (key < map->data[0].key) {
            ind = 0;
        } else if (key > map->data[map->count-1].key) {
            ind = map->count;
        } else {
            size_t il = 0;
            size_t iu = map->count-1;
            while ((iu-il) > 1) {
                size_t im = il+(iu-il)/2;
                intptr_t km = map->data[im].key;
                if (key > km) {
                    il = im;
                } else if (key < km) {
                    iu = im;
                } else {
                    il = (iu = im);
                    break;
                }
            }
            ind = iu;
        }
        if (map->data[ind].key == key) {
            map->data[ind].value = value;
        } else {
            intmap_insert(map, ind, key, value);
        }
    } else {
        intmap_insert(map, 0, key, value);
    }
}

void print_binary_u64 (u64 value) {
    u64 b = UINT64_C(1) << 63;
    do {
        if (value & b) {
            putchar('1');
        } else {
            putchar('0');
        }
        b >>= 1;
    } while (b);
}

int main (int argc, char ** argv) {
    IntMap_t memory;
    intmap_init(&memory, 0);

    char bitmask [37] = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
    do {
        char buf [BUFSIZ];
        char * s = fgets(buf, sizeof(buf), stdin);
        if (s == NULL) {
            break;
        }

        StringView_t line = sv_view_c_string(s);

        StringView_t target = sv_eat_until_space(&line);
        sv_eat_spaces(&line);
        char equals = sv_eat_char(&line);
        ASSERT(equals == '=');
        sv_eat_spaces(&line);
        StringView_t value = sv_eat_until_space(&line);

        if (sv_equals(&target, "mask")) {
            ASSERT(sv_length(&value) == 36);
            for (size_t i = 0; i < 36; ++i) {
                bitmask[i] = value.start[i];
            }
        } else {
            StringView_t mem = sv_eat_until_punctuation(&target);
            ASSERT(sv_equals(&mem, "mem"));
            char openbracket = sv_eat_char(&target);
            ASSERT(openbracket == '[');
            StringView_t index = sv_eat_until_punctuation(&target);
            char closebracket = sv_eat_char(&target);
            ASSERT(closebracket == ']');

            // TODO: Can't pass StringView_t to strtol et al, as they are not null-terminated
            unsigned long addr = strtoul(index.start, NULL, 10);
            unsigned long val = strtoul(value.start, NULL, 10);

            for (size_t i = 0; i < 36; ++i) {
                u64 b = UINT64_C(1) << (35-i);
                switch (bitmask[i]) {
                    case '0': {
                        // Do nothing
                    } break;
                    case '1': {
                        addr |= b;
                    } break;
                    case 'X': {
                        addr &= ~b;
                    } break;
                }
                b >>= 1;
            }

            Vector_t addrs;
            vector_init(&addrs, 1);
            vector_push_back(&addrs, addr);

            for (size_t i = 0; i < 36; ++i) {
                u64 b = UINT64_C(1) << (35-i);
                switch (bitmask[i]) {
                    case '0': {
                        // Do nothing
                    } break;
                    case '1': {
                        // Do nothing
                    } break;
                    case 'X': {
                        for (intptr_t const * it = vector_cbegin(&addrs), * end = vector_cend(&addrs); it != end; ++it) {
                            vector_push_back(&addrs, *it | b);
                        }
                    } break;
                }
                b >>= 1;
            }

            for (intptr_t const * it = vector_cbegin(&addrs), * end = vector_cend(&addrs); it != end; ++it) {
                intmap_set(&memory, *it, val);
            }

            vector_free(&addrs);
        }
    } while (1);

    size_t sum = 0;
    for (IntMapElement_t const * it = intmap_cbegin(&memory); it != intmap_cend(&memory); ++it) {
        sum += it->value;
    }

    DISP(sum);
}
