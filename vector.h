#ifndef VECTOR_H
#define VECTOR_H

#include "common.h"

typedef struct Vector {
    size_t capacity;
    size_t count;
    long * data;
} Vector_t;

void vector_init (struct Vector * v, size_t capacity) {
    v->capacity = capacity;
    v->count = 0;
    if (capacity) {
        v->data = malloc(sizeof(long) * capacity);
    } else {
        v->data = NULL;
    }
}

void vector_resize (struct Vector * v, size_t capacity) {
    if (capacity) {
        v->capacity = capacity;
        v->count = MIN(v->count, capacity);
        v->data = realloc(v->data, sizeof(long) * capacity);
    } else {
        free(v->data);
        v->capacity = 0;
        v->count = 0;
        v->data = NULL;
    }
}

void vector_grow (struct Vector * v) {
    size_t capacity = v->capacity + v->capacity / 2;
    vector_resize(v, MAX(capacity, 1));
}

void vector_ensure_capacity (struct Vector * v, size_t capacity) {
    while (v->capacity < capacity) {
        vector_grow(v);
    }
}

void vector_clear (struct Vector * v) {
    v->count = 0;
}

void vector_push_back (struct Vector * v, long x) {
    vector_ensure_capacity(v, v->count+1);
    v->data[v->count] = x;
    v->count += 1;
}

long vector_pop_back (struct Vector * v) {
    ASSERT(v->count);
    v->count -= 1;
    return v->data[v->count];
}

long vector_pop_front (struct Vector * v) {
    ASSERT(v->count);
    long x = v->data[0];
    v->count -= 1;
    memmove(&(v->data[0]), &(v->data[1]), v->count);
    return x;
}

void vector_insert (struct Vector * v, long x, size_t index) {
    ASSERT(index <= v->count);
    if (index == v->count) {
        vector_push_back(v, x);
    } else {
        vector_ensure_capacity(v, v->capacity + 1);
        memmove(&(v->data[index+1]), &(v->data[index]), v->count - index);
        v->data[index] = x;
        v->count += 1;
    }
}

void vector_insert_sorted (struct Vector * v, long x) {
    // TODO: Use binary search to speed this up
    size_t i = 0;
    while (i < v->count && v->data[i] < x) {
        i += 1;
    }
    vector_insert(v, x, i);
}

long * vector_begin (struct Vector * v) {
    return v->data;
}

long * vector_end (struct Vector * v) {
    return v->data + v->count;
}

void vector_sort (struct Vector * v) {
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

#endif
