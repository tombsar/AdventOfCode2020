#pragma once

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

struct Vector vector_copy (struct Vector const * v) {
    struct Vector vn;
    vector_init(&vn, v->capacity);
    memcpy(vn.data, v->data, v->count * sizeof(long));
    vn.count = v->count;
    return vn;
}

struct Vector vector_move (struct Vector * v) {
    struct Vector vn = *v;
    v->capacity = 0;
    v->count = 0;
    v->data = NULL;
    return vn;
}

long * vector_begin (struct Vector const * v) {
    return v->data;
}

long * vector_end (struct Vector const * v) {
    return v->data + v->count;
}

_Bool vector_is_sorted (struct Vector const * v) {
    for (size_t i = 1; i < v->count; ++i) {
        if (v->data[i] < v->data[i-1]) {
            return 0;
        }
    }
    return 1;
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
    size_t capacity = v->capacity;
    capacity = capacity + MAX(capacity / 2, 1);
    vector_resize(v, capacity);
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
    memmove(&(v->data[0]), &(v->data[1]), v->count*sizeof(long));
    return x;
}

void vector_insert (struct Vector * v, long x, size_t index) {
    ASSERT(index <= v->count);
    if (index == v->count) {
        vector_push_back(v, x);
    } else {
        vector_ensure_capacity(v, v->count+1);
        memmove(&(v->data[index+1]), &(v->data[index]), (v->count - index)*sizeof(long));
        v->data[index] = x;
        v->count += 1;
    }
}

void vector_insert_sorted (struct Vector * v, long x) {
    if (v->count) {
        ASSERT(vector_is_sorted(v));
        size_t i;
        if (x <= v->data[0]) {
            i = 0;
        } else if (x >= v->data[v->count-1]) {
            i = v->count;
        } else {
            size_t il = 0;
            size_t iu = v->count-1;
            while ((iu-il) > 1) {
                size_t im = il+(iu-il)/2;
                long xm = v->data[im];
                if (x > xm) {
                    il = im;
                } else if (x < xm) {
                    iu = im;
                } else {
                    il = (iu = im);
                    break;
                }
            }
            i = iu;
        }
        vector_insert(v, x, i);
    } else {
        vector_push_back(v, x);
    }
}

// TODO: Move this into a different header
void bubblesort (long * start, long * end) {
    size_t swaps;
    do {
        swaps = 0;
        for (long * p0 = start, * p1 = start+1; p1 != end; ++p0, ++p1) {
            if (*p1 > *p0) {
                long tmp = *p0;
                *p0 = *p1;
                *p1 = tmp;
                ++swaps;
            }
        }
    } while (swaps);
}

void vector_sort (struct Vector * v) {
    // TODO: Something better than bubble sort!
    bubblesort(vector_begin(v), vector_end(v));
}
