#pragma once

#include "common.h"
#include "vector.h"

typedef struct IntSet {
    struct Vector values;
} IntSet_t;

void intset_init (struct IntSet * set, size_t capacity) {
    ASSERT(set);
    vector_init(&set->values, capacity);
}

void intset_free (struct IntSet * set) {
    ASSERT(set);
    vector_free(&set->values);
}

void intset_copy (struct IntSet * dest, struct IntSet const * src) {
    ASSERT(src && dest);
    vector_copy(&dest->values, &src->values);
}

void intset_move (struct IntSet * dest, struct IntSet * src) {
    ASSERT(src && dest);
    vector_move(&dest->values, &src->values);
}

size_t intset_count (struct IntSet const * set) {
    ASSERT(set);
    return set->values.count;
}

_Bool intset_is_empty (struct IntSet const * set) {
    ASSERT(set);
    return (set->values.count == 0);
}

intptr_t intset_min (struct IntSet const * set) {
    ASSERT(set);
    ASSERT(set->values.count);
    return (set->values.data[0]);
}

intptr_t intset_max (struct IntSet const * set) {
    ASSERT(set);
    ASSERT(set->values.count);
    return (set->values.data[set->values.count-1]);
}

intptr_t const * intset_cbegin (struct IntSet const * set) {
    ASSERT(set);
    return vector_cbegin(&set->values);
}

intptr_t const * intset_cend (struct IntSet const * set) {
    ASSERT(set);
    return vector_cend(&set->values);
}

intptr_t * intset_begin (struct IntSet * set) {
    ASSERT(set);
    return vector_begin(&set->values);
}

intptr_t * intset_end (struct IntSet * set) {
    ASSERT(set);
    return vector_end(&set->values);
}

void intset_print (struct IntSet const * set) {
    ASSERT(set);
    printf("{");
    for (intptr_t const * it = vector_cbegin(&set->values); it != vector_cend(&set->values); ++it) {
        printf(" %zi", *it);
    }
    printf(" }\n");
}

void intset_clear (struct IntSet * set) {
    ASSERT(set);
    vector_clear(&set->values);
}

void intset_add (struct IntSet * set, intptr_t x) {
    ASSERT(set);
    ptrdiff_t i = vector_find_sorted(&set->values, x);
    if (i < 0) {
        vector_insert_sorted(&set->values, x);
    }
}

void intset_add_range (struct IntSet * set, intptr_t const * start, intptr_t const * end) {
    ASSERT(set);
    while (start != end) {
        intset_add(set, *start++);
    }
}

void intset_remove (struct IntSet * set, intptr_t x) {
    ASSERT(set);
    ptrdiff_t i = vector_find_sorted(&set->values, x);
    if (i >= 0) {
        vector_erase(&set->values, i);
    }
}

void intset_remove_range (struct IntSet * set, intptr_t const * start, intptr_t const * end) {
    ASSERT(set);
    while (start != end) {
        intset_remove(set, *start++);
    }
}

_Bool intset_contains (struct IntSet const * set, intptr_t x) {
    ASSERT(set);
    ptrdiff_t i = vector_find_sorted(&set->values, x);
    return (i >= 0);
}

struct IntSet intset_union (struct IntSet const * a, struct IntSet const * b) {
    ASSERT(a && b);
    struct IntSet result;
    // TODO: Is it faster to add more elements to a smaller set, or fewer elements to a larger set?
    if (intset_count(a) >= intset_count(b)) {
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
    if (intset_count(a) >= intset_count(b)) {
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
