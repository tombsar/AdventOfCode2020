#pragma once

typedef struct Deque {
    size_t capacity;
    size_t count;
    ptrdiff_t first;
    intptr_t * values;
} Deque_t;

void deque_init (struct Deque * deque, size_t capacity) {
    deque->capacity = capacity;
    deque->count = 0;
    deque->first = 0;
    if (capacity) {
        deque->values = malloc(sizeof(intptr_t)*capacity);
    } else {
        deque->values = NULL;
    }
}

void deque_copy (struct Deque * dest, struct Deque const * src) {
    deque_init(dest, src->capacity);
    dest->count = src->count;
    dest->first = src->first;
    memcpy(dest->values, src->values, sizeof(intptr_t)*src->capacity);
}

void deque_free (struct Deque * deque) {
    free(deque->values);
    deque->capacity = 0;
    deque->count = 0;
    deque->first = 0;
    deque->values = NULL;
}

void deque_push (struct Deque * deque, intptr_t value) {
    ASSERT(deque->count < deque->capacity);
    ptrdiff_t index = (deque->first + deque->count) % deque->capacity;
    deque->values[index] = value;
    deque->count += 1;
}

intptr_t deque_pop (struct Deque * deque) {
    ASSERT(deque->count);
    intptr_t value = deque->values[deque->first];
    deque->first = (deque->first + 1) % deque->capacity;
    deque->count -= 1;
    return value;
}
