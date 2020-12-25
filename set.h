#pragma once

#include "hash.h"

typedef struct Set {
    size_t capacity;
    size_t count;
    intptr_t * values;
} Set_t;

void set_init (struct Set * set, size_t capacity) {
    ASSERT(set);
    set->capacity = capacity;
    set->count = 0;
    if (capacity) {
        set->values = calloc(capacity, sizeof(intptr_t));
    } else {
        set->values = NULL;
    }
}

void set_free (struct Set * set) {
    set->capacity = 0;
    set->count = 0;
    free(set->values);
    set->values = NULL;
}

void set_resize (struct Set * set, size_t capacity) {
    if (capacity) {
        set->capacity = capacity;
        set->count = MIN(set->count, capacity);
        set->values = realloc(set->values, capacity*sizeof(intptr_t));
    } else {
        set_free(set);
    }
}

void set_copy (struct Set * dest, struct Set const * src) {
    ASSERT(dest);
    ASSERT(src);
    set_init(dest, src->capacity);
    memcpy(dest->values, src->values, src->count * sizeof(intptr_t));
    dest->count = src->count;
}

void set_move (struct Set * dest, struct Set * src) {
    ASSERT(dest);
    ASSERT(src);
    dest->capacity = src->capacity;
    dest->count = src->count;
    dest->values = src->values;
    src->capacity = 0;
    src->count = 0;
    src->values = NULL;
}

void set_grow (struct Set * set) {
    ASSERT(set);
    size_t capacity = set->capacity;
    capacity = capacity + MAX(capacity / 2, 1);
    set_resize(set, capacity);
}

void set_ensure_capacity (struct Set * set, size_t capacity) {
    ASSERT(set);
    while (set->capacity < capacity) {
        set_grow(set);
    }
}

intptr_t set_min (struct Set const * set) {
    ASSERT(set);
    ASSERT(set->count);
    return set->values[0];
}

intptr_t set_max (struct Set const * set) {
    ASSERT(set);
    ASSERT(set->count);
    return set->values[set->count-1];
}

intptr_t * set_begin (struct Set * set) {
    return &(set->values[0]);
}

intptr_t * set_end (struct Set * set) {
    return &(set->values[set->count]);
}

intptr_t const * set_cbegin (struct Set const * set) {
    return &(set->values[0]);
}

intptr_t const * set_cend (struct Set const * set) {
    return &(set->values[set->count]);
}

void set_print (struct Set const * set) {
    printf("Set with %zu elements: {\n", set->count);
    for (size_t i = 0; i < set->count; ++i) {
        printf("%8zi\n", set->values[i]);
    }
    printf("}\n");
}

ptrdiff_t set_find (struct Set const * set, intptr_t value) {
    if (set->count) {
        if (value >= set->values[0] && value <= set->values[set->count-1]) {
            intptr_t const * pl = &(set->values[0]);
            intptr_t const * pu = &(set->values[set->count]);
            while ((pu-pl) > 1) {
                intptr_t const * pm = pl + (pu-pl)/2;
                intptr_t km = *pm;
                if (value >= km) {
                    pl = pm;
                } else if (value < km) {
                    pu = pm;
                }
            }
            if (*pl == value) {
                return (pl-&(set->values[0]));
            }
        }
    }
    return -1;
}

intptr_t set_contains (struct Set const * set, intptr_t value) {
    return (set_find(set, value) != -1);
}

void set_insert (struct Set * set, size_t index, intptr_t value) {
    ASSERT(index <= set->count);
    set_ensure_capacity(set, set->count+1);
    if (index != set->count) {
        memmove(&(set->values[index+1]), &(set->values[index]), (set->count - index)*sizeof(intptr_t));
    }
    set->values[index] = value;
    set->count += 1;
}

void set_add (struct Set * set, intptr_t value) {
    if (set->count) {
        if (value < set->values[0]) {
            set_insert(set, 0, value);
        } else if (value > set->values[set->count-1]) {
            set_insert(set, set->count, value);
        } else {
            intptr_t const * pl = &(set->values[0]);
            intptr_t const * pu = &(set->values[set->count]);
            while ((pu-pl) > 1) {
                intptr_t const * pm = pl + (pu-pl)/2;
                intptr_t km = *pm;
                if (value >= km) {
                    pl = pm;
                } else {
                    pu = pm;
                }
            }
            ptrdiff_t ind = pl - &(set->values[0]);
            if (*pl != value) {
                set_insert(set, ind+1, value);
            }
        }
    } else {
        set_insert(set, 0, value);
    }
}

void set_remove (struct Set * set, intptr_t value) {
    ptrdiff_t index = set_find(set, value);
    if (index >= 0 && ((size_t)index+1) < set->count) {
        memmove(&(set->values[index]), &(set->values[index+1]), (set->count - index)*sizeof(intptr_t));
    }
    set->count -= 1;
}

struct Set set_union (struct Set const * a, struct Set const * b) {
    ASSERT(a && b);
    struct Set result;
    // TODO: Is it faster to add more elements to a smaller set, or fewer elements to a larger set?
    if (a->count >= b->count) {
        set_copy(&result, a);
        for (intptr_t const * it = set_cbegin(b); it != set_cend(b); ++it) {
            set_add(&result, *it);
        }
    } else {
        set_copy(&result, b);
        for (intptr_t const * it = set_cbegin(a); it != set_cend(a); ++it) {
            set_add(&result, *it);
        }
    }
    return result;
}

struct Set set_intersection (struct Set const * a, struct Set const * b) {
    ASSERT(a && b);
    struct Set result;
    set_init(&result, 0);
    if (a->count >= b->count) {
        for (intptr_t const * it = set_cbegin(b); it != set_cend(b); ++it) {
            if (set_contains(a, *it)) {
                set_add(&result, *it);
            }
        }
    } else {
        for (intptr_t const * it = set_cbegin(a); it != set_cend(a); ++it) {
            if (set_contains(b, *it)) {
                set_add(&result, *it);
            }
        }
    }
    return result;
}

struct Set set_difference (struct Set const * a, struct Set const * b) {
    ASSERT(a && b);
    struct Set result;
    set_init(&result, 0);
    for (intptr_t const * it = set_cbegin(a); it != set_cend(a); ++it) {
        if (!set_contains(b, *it)) {
            set_add(&result, *it);
        }
    }
    return result;
}

typedef struct HashSet {
    size_t n_bins;
    struct Set * bins;
} HashSet_t;

void hashset_init (struct HashSet * set, size_t n_bins, size_t capacity) {
    ASSERT(set);
    ASSERT(n_bins);
    set->n_bins = n_bins;
    set->bins = calloc(n_bins, sizeof(struct Set));
    for (size_t i = 0; i < n_bins; ++i) {
        set_init(&(set->bins[i]), capacity);
    }
}

void hashset_free (struct HashSet * set) {
    for (size_t i = 0; i < set->n_bins; ++i) {
        set_free(&(set->bins[i]));
    }
    free(set->bins);
    set->bins = NULL;
    set->n_bins = 0;
}

void hashset_copy (struct HashSet * dest, struct HashSet const * src) {
    // TODO: Simplify this
    ASSERT(dest);
    ASSERT(src);
    hashset_init(dest, src->n_bins, 0);
    for (size_t i = 0; i < src->n_bins; ++i) {
        set_copy(&(dest->bins[i]), &(src->bins[i]));
    }
}

void hashset_move (struct HashSet * dest, struct HashSet * src) {
    ASSERT(dest);
    ASSERT(src);
    dest->n_bins = src->n_bins;
    dest->bins = src->bins;
    src->n_bins = 0;
    src->bins = NULL;
}

size_t hashset_count (struct HashSet const * set) {
    size_t n = 0;
    for (size_t i = 0; i < set->n_bins; ++i) {
        n += set->bins[i].count;
    }
    return n;
}

_Bool hashset_contains (struct HashSet const * set, intptr_t value) {
    size_t bin_index = HASH(value) % set->n_bins;
    return set_contains(&(set->bins[bin_index]), value);
}

void hashset_add (struct HashSet * set, intptr_t value) {
    size_t bin_index = HASH(value) % set->n_bins;
    set_add(&(set->bins[bin_index]), value);
}
