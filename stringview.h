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

_Bool sv_equals (struct StringView const * s, char const * str) {
    for (char const * p = s->start; p < s->end; ++p, ++str) {
        if (*p != *str) {
            return 0;
        }
    }
    return (*str == '\0');
}

void sv_print (struct StringView const * s) {
    putchar('"');
    for (char const * p = s->start; p < s->end; ++p) {
        putchar(*p);
    }
    putchar('"');
}

char const * sv_make_c_string (struct StringView const * s) {
    return make_c_string(s->start, s->end);
}

size_t sv_count (struct StringView * s, char c) {
    size_t n = 0;
    for (char const * it = s->start, * end = s->end; it != end; ++it) {
        if (*it == c) {
            n += 1;
        }
    }
    return n;
}

char sv_eat_char (struct StringView * s) {
    char c = s->start[0];
    s->start = &(s->start[1]);
    return c;
}

struct StringView sv_eat_spaces (struct StringView * s) {
    char const * p = s->start;
    while (p < s->end && isspace(*p)) {
        ++p;
    }
    struct StringView left = {
        .start = s->start,
        .end = p
    };
    s->start = p;
    return left;
}

struct StringView sv_eat_digits (struct StringView * s) {
    char const * p = s->start;
    while (p < s->end && isdigit(*p)) {
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
    while (p < s->end && *p != c) {
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
    while (p < s->end && !isspace(*p)) {
        ++p;
    }
    struct StringView left = {
        .start = s->start,
        .end = p
    };
    s->start = p;
    return left;
}

struct StringView sv_eat_until_punctuation (struct StringView * s) {
    char const * p = s->start;
    while (p < s->end && !ispunct(*p)) {
        ++p;
    }
    struct StringView left = {
        .start = s->start,
        .end = p
    };
    s->start = p;
    return left;
}
