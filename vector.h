#ifndef VECTOR_H
#define VECTOR_H

#include "common.h"

typedef struct Vector {
    size_t capacity;
    size_t count;
    long * data;
} Vector_t;

void vector_init (struct Vector * v, size_t initial_capacity) {
    v->capacity = initial_capacity;
    v->count = 0;
    v->data = calloc(initial_capacity, sizeof(long));
}

void vector_resize (struct Vector * v, size_t new_capacity) {
    // TODO: What to do if the call would discard data?
    ASSERT(new_capacity >= v->count);
    v->capacity = new_capacity;
    v->data = realloc(v->data, sizeof(long) * new_capacity);
}

void vector_clear (struct Vector * v) {
    v->count = 0;
}

void vector_push_back (struct Vector * v, long x) {
    if (v->count == v->capacity) {
        // TODO: Use a 3/2 factor instead?
        vector_resize(v, MAX(v->capacity * 2, 1));
    }
    ASSERT(v->capacity > v->count);
    v->data[v->count++] = x;
}

long vector_pop_back (struct Vector * v) {
    ASSERT(v->count);
    return v->data[--v->count];
}

long * vector_begin (struct Vector * v) {
    return v->data;
}

long * vector_end (struct Vector * v) {
    return v->data + v->count;
}

void vector_sort_ascending (struct Vector * v) {
    // TODO: Something better than bubble sort!
    size_t swaps;
    do {
        swaps = 0;
        for (size_t i = 1; i < v->count; ++i) {
            if (v->data[i-1] > v->data[i]) {
                long tmp = v->data[i-1];
                v->data[i-1] = v->data[i];
                v->data[i] = tmp;
                ++swaps;
            }
        }
    } while (swaps);
}

void vector_sort_descending (struct Vector * v) {
    // TODO: Something better than bubble sort!
    size_t swaps;
    do {
        swaps = 0;
        for (size_t i = 1; i < v->count; ++i) {
            if (v->data[i-1] < v->data[i]) {
                long tmp = v->data[i-1];
                v->data[i-1] = v->data[i];
                v->data[i] = tmp;
                ++swaps;
            }
        }
    } while (swaps);
}

#endif
