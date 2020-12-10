#pragma once

#include "common.h"
#include "vector.h"

typedef struct IntSet {
    struct Vector values;
} IntSet_t;

void intset_init (struct IntSet * set, size_t capacity) {
    vector_init(&set->values, capacity);
}

void intset_free (struct IntSet * set) {
    vector_free(&set->values);
}

struct IntSet intset_copy (struct IntSet const * other) {
    struct IntSet set;
    set.values = vector_copy(&other->values);
    return set;
}

struct IntSet intset_move (struct IntSet * other) {
    struct IntSet set;
    set.values = vector_move(&other->values);
    return set;
}

size_t intset_count (struct IntSet const * set) {
    return set->values.count;
}

_Bool intset_is_empty (struct IntSet const * set) {
    return (set->values.count == 0);
}

intptr_t intset_min (struct IntSet const * set) {
    ASSERT(set->values.count);
    return (set->values.data[0]);
}

intptr_t intset_max (struct IntSet const * set) {
    ASSERT(set->values.count);
    return (set->values.data[set->values.count-1]);
}

void intset_print (struct IntSet const * set) {
    printf("{");
    for (intptr_t * it = vector_begin(&set->values); it != vector_end(&set->values); ++it) {
        printf(" %zi", *it);
    }
    printf(" }\n");
}

intptr_t * intset_begin (struct IntSet const * set) {
    return vector_begin(&set->values);
}

intptr_t * intset_end (struct IntSet const * set) {
    return vector_end(&set->values);
}

void intset_clear (struct IntSet * set) {
    vector_clear(&set->values);
}

void intset_add (struct IntSet * set, intptr_t x) {
    ptrdiff_t i = vector_find_sorted(&set->values, x);
    if (i < 0) {
        vector_insert_sorted(&set->values, x);
    }
}

void intset_remove (struct IntSet * set, intptr_t x) {
    ptrdiff_t i = vector_find_sorted(&set->values, x);
    if (i >= 0) {
        vector_erase(&set->values, i);
    }
}

_Bool intset_contains (struct IntSet const * set, intptr_t x) {
    ptrdiff_t i = vector_find_sorted(&set->values, x);
    return (i >= 0);
}

struct IntSet intset_union (struct IntSet const * a, struct IntSet const * b) {
    struct IntSet result;
    // TODO: Is it faster to add more elements to a smaller set, or fewer elements to a larger set?
    if (intset_count(a) >= intset_count(b)) {
        result = intset_copy(a);
        for (intptr_t const * it = intset_begin(b); it != intset_end(b); ++it) {
            intset_add(&result, *it);
        }
    } else {
        result = intset_copy(b);
        for (intptr_t const * it = intset_begin(a); it != intset_end(a); ++it) {
            intset_add(&result, *it);
        }
    }
    return result;
}

struct IntSet intset_intersection (struct IntSet const * a, struct IntSet const * b) {
    struct IntSet result;
    intset_init(&result, 0);
    if (intset_count(a) >= intset_count(b)) {
        for (intptr_t const * it = intset_begin(b); it != intset_end(b); ++it) {
            if (intset_contains(a, *it)) {
                intset_add(&result, *it);
            }
        }
    } else {
        for (intptr_t const * it = intset_begin(a); it != intset_end(a); ++it) {
            if (intset_contains(b, *it)) {
                intset_add(&result, *it);
            }
        }
    }
    return result;
}

struct IntSet intset_difference (struct IntSet const * a, struct IntSet const * b) {
    struct IntSet result;
    intset_init(&result, 0);
    for (intptr_t const * it = intset_begin(a); it != intset_end(a); ++it) {
        if (!intset_contains(b, *it)) {
            intset_add(&result, *it);
        }
    }
    return result;
}
