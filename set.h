#pragma once

#include "hash.h"

typedef struct IntSet {
    size_t capacity;
    size_t count;
    intptr_t * values;
} IntSet_t;

void intset_init (struct IntSet * set, size_t capacity) {
    ASSERT(set);
    set->capacity = capacity;
    set->count = 0;
    if (capacity) {
        set->values = calloc(capacity, sizeof(intptr_t));
    } else {
        set->values = NULL;
    }
}

void intset_free (struct IntSet * set) {
    set->capacity = 0;
    set->count = 0;
    free(set->values);
    set->values = NULL;
}

void intset_resize (struct IntSet * set, size_t capacity) {
    if (capacity) {
        set->capacity = capacity;
        set->count = MIN(set->count, capacity);
        set->values = realloc(set->values, capacity*sizeof(intptr_t));
    } else {
        intset_free(set);
    }
}

void intset_copy (struct IntSet * dest, struct IntSet const * src) {
    ASSERT(dest);
    ASSERT(src);
    intset_init(dest, src->capacity);
    memcpy(dest->values, src->values, src->count * sizeof(intptr_t));
    dest->count = src->count;
}

void intset_move (struct IntSet * dest, struct IntSet * src) {
    ASSERT(dest);
    ASSERT(src);
    dest->capacity = src->capacity;
    dest->count = src->count;
    dest->values = src->values;
    src->capacity = 0;
    src->count = 0;
    src->values = NULL;
}

void intset_grow (struct IntSet * set) {
    ASSERT(set);
    size_t capacity = set->capacity;
    capacity = capacity + MAX(capacity / 2, 1);
    intset_resize(set, capacity);
}

void intset_ensure_capacity (struct IntSet * set, size_t capacity) {
    ASSERT(set);
    while (set->capacity < capacity) {
        intset_grow(set);
    }
}

intptr_t intset_min (struct IntSet const * set) {
    ASSERT(set);
    ASSERT(set->count);
    return set->values[0];
}

intptr_t intset_max (struct IntSet const * set) {
    ASSERT(set);
    ASSERT(set->count);
    return set->values[set->count-1];
}

intptr_t * intset_begin (struct IntSet * set) {
    return &(set->values[0]);
}

intptr_t * intset_end (struct IntSet * set) {
    return &(set->values[set->count]);
}

intptr_t const * intset_cbegin (struct IntSet const * set) {
    return &(set->values[0]);
}

intptr_t const * intset_cend (struct IntSet const * set) {
    return &(set->values[set->count]);
}

void intset_print (struct IntSet const * set) {
    printf("IntSet with %zu elements: {\n", set->count);
    for (size_t i = 0; i < set->count; ++i) {
        printf("%8zi\n", set->values[i]);
    }
    printf("}\n");
}

ptrdiff_t intset_find (struct IntSet const * set, intptr_t value) {
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

intptr_t intset_contains (struct IntSet const * set, intptr_t value) {
    return (intset_find(set, value) != -1);
}

void intset_insert (struct IntSet * set, size_t index, intptr_t value) {
    ASSERT(index <= set->count);
    intset_ensure_capacity(set, set->count+1);
    if (index != set->count) {
        memmove(&(set->values[index+1]), &(set->values[index]), (set->count - index)*sizeof(intptr_t));
    }
    set->values[index] = value;
    set->count += 1;
}

void intset_add (struct IntSet * set, intptr_t value) {
    if (set->count) {
        if (value < set->values[0]) {
            intset_insert(set, 0, value);
        } else if (value > set->values[set->count-1]) {
            intset_insert(set, set->count, value);
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
                intset_insert(set, ind+1, value);
            }
        }
    } else {
        intset_insert(set, 0, value);
    }
}

void intset_remove (struct IntSet * set, intptr_t value) {
    ptrdiff_t index = intset_find(set, value);
    if (index >= 0 && ((size_t)index+1) < set->count) {
        memmove(&(set->values[index]), &(set->values[index+1]), (set->count - index)*sizeof(intptr_t));
    }
    set->count -= 1;
}

struct IntSet intset_union (struct IntSet const * a, struct IntSet const * b) {
    ASSERT(a && b);
    struct IntSet result;
    // TODO: Is it faster to add more elements to a smaller set, or fewer elements to a larger set?
    if (a->count >= b->count) {
        intset_copy(&result, a);
        for (intptr_t const * it = intset_cbegin(b); it != intset_cend(b); ++it) {
            intset_add(&result, *it);
        }
    } else {
        intset_copy(&result, b);
        for (intptr_t const * it = intset_cbegin(a); it != intset_cend(a); ++it) {
            intset_add(&result, *it);
        }
    }
    return result;
}

struct IntSet intset_intersection (struct IntSet const * a, struct IntSet const * b) {
    ASSERT(a && b);
    struct IntSet result;
    intset_init(&result, 0);
    if (a->count >= b->count) {
        for (intptr_t const * it = intset_cbegin(b); it != intset_cend(b); ++it) {
            if (intset_contains(a, *it)) {
                intset_add(&result, *it);
            }
        }
    } else {
        for (intptr_t const * it = intset_cbegin(a); it != intset_cend(a); ++it) {
            if (intset_contains(b, *it)) {
                intset_add(&result, *it);
            }
        }
    }
    return result;
}

struct IntSet intset_difference (struct IntSet const * a, struct IntSet const * b) {
    ASSERT(a && b);
    struct IntSet result;
    intset_init(&result, 0);
    for (intptr_t const * it = intset_cbegin(a); it != intset_cend(a); ++it) {
        if (!intset_contains(b, *it)) {
            intset_add(&result, *it);
        }
    }
    return result;
}

typedef struct IntHashSet {
    size_t n_bins;
    struct IntSet * bins;
} IntHashSet_t;

void inthashset_init (struct IntHashSet * set, size_t n_bins, size_t capacity) {
    ASSERT(set);
    ASSERT(n_bins);
    set->n_bins = n_bins;
    set->bins = calloc(n_bins, sizeof(struct IntSet));
    for (size_t i = 0; i < n_bins; ++i) {
        intset_init(&(set->bins[i]), capacity);
    }
}

void inthashset_free (struct IntHashSet * set) {
    for (size_t i = 0; i < set->n_bins; ++i) {
        intset_free(&(set->bins[i]));
    }
    free(set->bins);
    set->bins = NULL;
    set->n_bins = 0;
}

void inthashset_copy (struct IntHashSet * dest, struct IntHashSet const * src) {
    // TODO: Simplify this
    ASSERT(dest);
    ASSERT(src);
    inthashset_init(dest, src->n_bins, 0);
    for (size_t i = 0; i < src->n_bins; ++i) {
        intset_copy(&(dest->bins[i]), &(src->bins[i]));
    }
}

void inthashset_move (struct IntHashSet * dest, struct IntHashSet * src) {
    ASSERT(dest);
    ASSERT(src);
    dest->n_bins = src->n_bins;
    dest->bins = src->bins;
    src->n_bins = 0;
    src->bins = NULL;
}

size_t inthashset_count (struct IntHashSet const * set) {
    size_t n = 0;
    for (size_t i = 0; i < set->n_bins; ++i) {
        n += set->bins[i].count;
    }
    return n;
}

_Bool inthashset_contains (struct IntHashSet const * set, intptr_t value) {
    size_t bin_index = HASH(value) % set->n_bins;
    return intset_contains(&(set->bins[bin_index]), value);
}

void inthashset_add (struct IntHashSet * set, intptr_t value) {
    size_t bin_index = HASH(value) % set->n_bins;
    intset_add(&(set->bins[bin_index]), value);
}
