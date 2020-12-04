#pragma once

#include "common.h"

typedef struct StringView {
    char const * start;
    char const * end;
} StringView_t;

struct StringView sv_view_c_string (char const * str) {
    char const * p = str;
    do {
        ++p;
    } while (*p != '\0');
    struct StringView sv = {
        .start = str,
        .end = p
    };
    return sv;
}

size_t sv_length (struct StringView const * s) {
    ASSERT(s->end >= s->start);
    return (s->end - s->start);
}

_Bool sv_is_empty (struct StringView const * s) {
    return !(s->end > s->start);
}

void sv_print (struct StringView const * s) {
    putchar('"');
    for (char const * p = s->start; p != s->end; ++p) {
        putchar(*p);
    }
    putchar('"');
}

char const * sv_make_c_string (struct StringView const * s) {
    return make_c_string(s->start, s->end);
}

struct StringView sv_eat_char (struct StringView * s) {
    struct StringView left = {
        .start = s->start,
        .end = s->start+1
    };
    s->start = s->start+1;
    return left;
}

struct StringView sv_eat_space (struct StringView * s) {
    char const * p = s->start;
    while (p != s->end && isspace(*p)) {
        ++p;
    }
    struct StringView left = {
        .start = s->start,
        .end = p
    };
    s->start = p;
    return left;
}

struct StringView sv_eat_until (struct StringView * s, char c) {
    char const * p = s->start;
    while (p != s->end && *p != c) {
        ++p;
    }
    struct StringView left = {
        .start = s->start,
        .end = p
    };
    s->start = p;
    return left;
}

struct StringView sv_eat_until_space (struct StringView * s) {
    char const * p = s->start;
    while (p != s->end && !isspace(*p)) {
        ++p;
    }
    struct StringView left = {
        .start = s->start,
        .end = p
    };
    s->start = p;
    return left;
}
