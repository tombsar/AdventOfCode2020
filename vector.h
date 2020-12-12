#pragma once

#include "common.h"

typedef struct Vector {
    size_t capacity;
    size_t count;
    intptr_t * data;
} Vector_t;

void vector_init (struct Vector * v, size_t capacity) {
    ASSERT(v);
    v->capacity = capacity;
    v->count = 0;
    if (capacity) {
        v->data = malloc(sizeof(intptr_t) * capacity);
    } else {
        v->data = NULL;
    }
}

void vector_free (struct Vector * v) {
    v->capacity = 0;
    v->count = 0;
    free(v->data);
    v->data = 0;
}

void vector_copy (struct Vector * dest, struct Vector const * src) {
    ASSERT(dest);
    ASSERT(src);
    vector_init(dest, src->capacity);
    memcpy(dest->data, src->data, src->count * sizeof(intptr_t));
    dest->count = src->count;
}

void vector_move (struct Vector * dest, struct Vector * src) {
    ASSERT(dest);
    ASSERT(src);
    dest->capacity = src->capacity;
    dest->count = src->count;
    dest->data = src->data;
    src->capacity = 0;
    src->count = 0;
    src->data = NULL;
}

intptr_t const * vector_cbegin (struct Vector const * v) {
    ASSERT(v);
    return v->data;
}

intptr_t const * vector_cend (struct Vector const * v) {
    ASSERT(v);
    return v->data + v->count;
}

intptr_t * vector_begin (struct Vector * v) {
    ASSERT(v);
    return v->data;
}

intptr_t * vector_end (struct Vector * v) {
    ASSERT(v);
    return v->data + v->count;
}

intptr_t vector_min (struct Vector const * v) {
    ASSERT(v);
    ASSERT(v->count);
    intptr_t minval = v->data[0];
    for (size_t i = 1; i < v->count; ++i) {
        minval = MIN(minval, v->data[i]);
    }
    return minval;
}

intptr_t vector_max (struct Vector const * v) {
    ASSERT(v);
    ASSERT(v->count);
    intptr_t maxval = v->data[0];
    for (size_t i = 1; i < v->count; ++i) {
        maxval = MAX(maxval, v->data[i]);
    }
    return maxval;
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

ptrdiff_t vector_find (struct Vector const * v, intptr_t x) {
    ASSERT(v);
    for (size_t i = 0; i < v->count; ++i) {
        if (v->data[i] == x) {
            return (ptrdiff_t)i;
        }
    }
    return -1;
}

ptrdiff_t vector_find_sorted (struct Vector const * v, intptr_t x) {
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
                intptr_t xm = v->data[im];
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
        v->data = realloc(v->data, sizeof(intptr_t) * capacity);
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

void vector_push_back (struct Vector * v, intptr_t x) {
    ASSERT(v);
    vector_ensure_capacity(v, v->count+1);
    v->data[v->count] = x;
    v->count += 1;
}

intptr_t vector_pop_back (struct Vector * v) {
    ASSERT(v->count);
    v->count -= 1;
    return v->data[v->count];
}

intptr_t vector_pop_front (struct Vector * v) {
    ASSERT(v);
    ASSERT(v->count);
    intptr_t x = v->data[0];
    v->count -= 1;
    memmove(&(v->data[0]), &(v->data[1]), v->count*sizeof(intptr_t));
    return x;
}

void vector_insert (struct Vector * v, intptr_t x, size_t index) {
    ASSERT(v);
    ASSERT(index <= v->count);
    if (index == v->count) {
        vector_push_back(v, x);
    } else {
        vector_ensure_capacity(v, v->count+1);
        memmove(&(v->data[index+1]), &(v->data[index]), (v->count - index)*sizeof(intptr_t));
        v->data[index] = x;
        v->count += 1;
    }
}

void vector_insert_sorted (struct Vector * v, intptr_t x) {
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
                intptr_t xm = v->data[im];
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
            memmove(&(v->data[i]), &(v->data[i+1]), v->count*sizeof(intptr_t));
        }
    }
}

// TODO: Move this into a different header
void bubblesort (intptr_t * start, intptr_t * end) {
    size_t swaps;
    do {
        swaps = 0;
        for (intptr_t * p0 = start, * p1 = start+1; p1 != end; ++p0, ++p1) {
            if (*p1 > *p0) {
                intptr_t tmp = *p0;
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
