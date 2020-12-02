#pragma once

#include "common.h"

typedef struct Vector {
    size_t capacity;
    size_t count;
    s32 * data;
} Vector_t;

void vector_init (struct Vector * v, size_t capacity) {
    ASSERT(v);
    v->capacity = capacity;
    v->count = 0;
    if (capacity) {
        v->data = malloc(sizeof(s32) * capacity);
    } else {
        v->data = NULL;
    }
}

struct Vector vector_copy (struct Vector const * v) {
    ASSERT(v);
    struct Vector vn;
    vector_init(&vn, v->capacity);
    memcpy(vn.data, v->data, v->count * sizeof(s32));
    vn.count = v->count;
    return vn;
}

struct Vector vector_move (struct Vector * v) {
    ASSERT(v);
    struct Vector vn = *v;
    v->capacity = 0;
    v->count = 0;
    v->data = NULL;
    return vn;
}

s32 * vector_begin (struct Vector const * v) {
    ASSERT(v);
    return v->data;
}

s32 * vector_end (struct Vector const * v) {
    ASSERT(v);
    return v->data + v->count;
}

_Bool vector_is_sorted (struct Vector const * v) {
    ASSERT(v);
    for (size_t i = 1; i < v->count; ++i) {
        if (v->data[i] < v->data[i-1]) {
            return 0;
        }
    }
    return 1;
}

ptrdiff_t vector_find (struct Vector const * v, s32 x) {
    ASSERT(v);
    for (size_t i = 0; i < v->count; ++i) {
        if (v->data[i] == x) {
            return (ptrdiff_t)i;
        }
    }
    return -1;
}

ptrdiff_t vector_find_sorted (struct Vector * v, s32 x) {
    ASSERT(v);
    ASSERT(vector_is_sorted(v));
    if (v->count) {
        if (x < v->data[0] || x > v->data[v->count-1]) {
            return -1;
        } else {
            ptrdiff_t il = 0;
            ptrdiff_t iu = v->count;
            while (iu > il) {
                ptrdiff_t im = il + (iu-il)/2;
                s32 xm = v->data[im];
                if (x < xm) {
                    iu = im;
                } else if (x > xm) {
                    il = im+1;
                } else {
                    return im;
                }
            }
        }
    }
    return -1;
}

void vector_resize (struct Vector * v, size_t capacity) {
    ASSERT(v);
    if (capacity) {
        v->capacity = capacity;
        v->count = MIN(v->count, capacity);
        v->data = realloc(v->data, sizeof(s32) * capacity);
    } else {
        free(v->data);
        v->capacity = 0;
        v->count = 0;
        v->data = NULL;
    }
}

void vector_grow (struct Vector * v) {
    ASSERT(v);
    size_t capacity = v->capacity;
    capacity = capacity + MAX(capacity / 2, 1);
    vector_resize(v, capacity);
}

void vector_ensure_capacity (struct Vector * v, size_t capacity) {
    ASSERT(v);
    while (v->capacity < capacity) {
        vector_grow(v);
    }
}

void vector_clear (struct Vector * v) {
    ASSERT(v);
    v->count = 0;
}

void vector_push_back (struct Vector * v, s32 x) {
    ASSERT(v);
    vector_ensure_capacity(v, v->count+1);
    v->data[v->count] = x;
    v->count += 1;
}

s32 vector_pop_back (struct Vector * v) {
    ASSERT(v->count);
    v->count -= 1;
    return v->data[v->count];
}

s32 vector_pop_front (struct Vector * v) {
    ASSERT(v);
    ASSERT(v->count);
    s32 x = v->data[0];
    v->count -= 1;
    memmove(&(v->data[0]), &(v->data[1]), v->count*sizeof(s32));
    return x;
}

void vector_insert (struct Vector * v, s32 x, size_t index) {
    ASSERT(v);
    ASSERT(index <= v->count);
    if (index == v->count) {
        vector_push_back(v, x);
    } else {
        vector_ensure_capacity(v, v->count+1);
        memmove(&(v->data[index+1]), &(v->data[index]), (v->count - index)*sizeof(s32));
        v->data[index] = x;
        v->count += 1;
    }
}

void vector_insert_sorted (struct Vector * v, s32 x) {
    ASSERT(v);
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
                s32 xm = v->data[im];
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

void vector_erase (struct Vector * v, size_t i) {
    ASSERT(v);
    ASSERT(i < v->count);
    if (i == v->count-1) {
        vector_pop_back(v);
    } else {
        v->count -= 1;
        if (v->count) {
            memmove(&(v->data[i]), &(v->data[i+1]), v->count*sizeof(s32));
        }
    }
}

// TODO: Move this into a different header
void bubblesort (s32 * start, s32 * end) {
    size_t swaps;
    do {
        swaps = 0;
        for (s32 * p0 = start, * p1 = start+1; p1 != end; ++p0, ++p1) {
            if (*p1 > *p0) {
                s32 tmp = *p0;
                *p0 = *p1;
                *p1 = tmp;
                ++swaps;
            }
        }
    } while (swaps);
}

void vector_sort (struct Vector * v) {
    ASSERT(v);
    // TODO: Something better than bubble sort!
    bubblesort(vector_begin(v), vector_end(v));
}
