#pragma once

#include "common.h"
#include "stringview.h"

typedef struct Glossary {
    size_t strings_capacity;
    size_t strings_index;
    char * strings;

    size_t ids_capacity;
    size_t n_ids;
    intptr_t * ids;
} Glossary_t;

void glossary_init (struct Glossary * g) {
    g->strings_capacity = 0;
    g->strings_index = 0;
    g->strings = NULL;

    g->ids_capacity = 0;
    g->n_ids = 0;
    g->ids = NULL;
}

void glossary_strings_storage_resize (struct Glossary * g, size_t capacity) {
    if (capacity) {
        g->strings_capacity = capacity;
        g->strings = realloc(g->strings, sizeof(char)*capacity);
    } else {
        free(g->strings);
        g->strings_capacity = 0;
        g->strings_index = 0;
        g->strings = NULL;
    }
}

void glossary_strings_storage_grow (struct Glossary * g) {
    size_t capacity = g->strings_capacity;
    capacity += MAX(capacity / 2, 1);
    glossary_strings_storage_resize(g, capacity);
}

void glossary_strings_storage_ensure_capacity (struct Glossary * g, size_t capacity) {
    while (g->strings_capacity < capacity) {
        glossary_strings_storage_grow(g);
    }
}

intptr_t glossary_strings_storage_push (struct Glossary * g, StringView_t sv) {
    intptr_t id = g->strings_index;
    size_t len = sv_length(&sv);
    glossary_strings_storage_ensure_capacity(g, id+len+1);
    memcpy(&(g->strings[id]), sv.start, len);
    g->strings[id+len] = '\0';
    g->strings_index += len+1;
    return id;
}

void glossary_ids_resize (struct Glossary * g, size_t capacity) {
    if (capacity) {
        g->ids_capacity = capacity;
        g->ids = realloc(g->ids, sizeof(intptr_t)*capacity);
    } else {
        free(g->ids);
        g->ids_capacity = 0;
        g->n_ids = 0;
        g->ids = NULL;
    }
}

void glossary_ids_grow (struct Glossary * g) {
    size_t capacity = g->ids_capacity;
    capacity += MAX(capacity / 2, 1);
    glossary_ids_resize(g, capacity);
}

void glossary_ids_ensure_capacity (struct Glossary * g, size_t capacity) {
    while (g->ids_capacity < capacity) {
        glossary_ids_grow(g);
    }
}

void glossary_ids_push (struct Glossary * g, intptr_t id) {
    // TODO: Insert in sorted order
    glossary_ids_ensure_capacity(g, g->n_ids + 1);
    g->ids[g->n_ids] = id;
    g->n_ids += 1;
}

char const * glossary_get_string (struct Glossary * g, intptr_t id) {
    ASSERT(id >= 0 && (size_t)id <= g->strings_index);
    return &(g->strings[id]);
}

intptr_t glossary_find (struct Glossary * g, StringView_t sv) {
    // TODO: Something better than linear search
    for (size_t i = 0; i < g->n_ids; ++i) {
        if (sv_equals(&sv, glossary_get_string(g, g->ids[i]))) {
            return g->ids[i];
        }
    }
    return -1;
}

intptr_t glossary_add (struct Glossary * g, StringView_t sv) {
    intptr_t id = glossary_find(g, sv);
    if (id < 0) {
        id = glossary_strings_storage_push(g, sv);
        glossary_ids_push(g, id);
    }
    return id;
}

intptr_t glossary_add_c_string (struct Glossary * g, char const * str) {
    return glossary_add(g, sv_view_c_string(str));
}
