#include "common.h"
#include "stringview.h"
#include "map.h"
#include "vector.h"

void print_binary_u64 (u64 value) {
    u64 b = UINT64_C(1) << 63;
    do {
        if (value & b) {
            putchar('1');
        } else {
            putchar('0');
        }
        b >>= 1;
    } while (b);
}

int main (int argc, char ** argv) {
    IntMap_t memory;
    intmap_init(&memory, 0);

    char bitmask [37] = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
    do {
        char buf [BUFSIZ];
        char * s = fgets(buf, sizeof(buf), stdin);
        if (s == NULL) {
            break;
        }

        StringView_t line = sv_view_c_string(s);

        StringView_t target = sv_eat_until_space(&line);
        sv_eat_spaces(&line);
        char equals = sv_eat_char(&line);
        ASSERT(equals == '=');
        sv_eat_spaces(&line);
        StringView_t value = sv_eat_until_space(&line);

        if (sv_equals(&target, "mask")) {
            ASSERT(sv_length(&value) == 36);
            for (size_t i = 0; i < 36; ++i) {
                bitmask[i] = value.start[i];
            }
        } else {
            StringView_t mem = sv_eat_until_punctuation(&target);
            ASSERT(sv_equals(&mem, "mem"));
            char openbracket = sv_eat_char(&target);
            ASSERT(openbracket == '[');
            StringView_t index = sv_eat_until_punctuation(&target);
            char closebracket = sv_eat_char(&target);
            ASSERT(closebracket == ']');

            // TODO: Can't pass StringView_t to strtol et al, as they are not null-terminated
            unsigned long addr = strtoul(index.start, NULL, 10);
            unsigned long val = strtoul(value.start, NULL, 10);

            for (size_t i = 0; i < 36; ++i) {
                u64 b = UINT64_C(1) << (35-i);
                switch (bitmask[i]) {
                    case '0': {
                        // Do nothing
                    } break;
                    case '1': {
                        addr |= b;
                    } break;
                    case 'X': {
                        addr &= ~b;
                    } break;
                }
                b >>= 1;
            }

            Vector_t addrs;
            vector_init(&addrs, 1);
            vector_push_back(&addrs, addr);

            for (size_t i = 0; i < 36; ++i) {
                u64 b = UINT64_C(1) << (35-i);
                switch (bitmask[i]) {
                    case '0': {
                        // Do nothing
                    } break;
                    case '1': {
                        // Do nothing
                    } break;
                    case 'X': {
                        for (intptr_t const * it = vector_cbegin(&addrs), * end = vector_cend(&addrs); it != end; ++it) {
                            vector_push_back(&addrs, *it | b);
                        }
                    } break;
                }
                b >>= 1;
            }

            for (intptr_t const * it = vector_cbegin(&addrs), * end = vector_cend(&addrs); it != end; ++it) {
                intmap_set(&memory, *it, val);
            }

            vector_free(&addrs);
        }
    } while (1);

    size_t sum = 0;
    for (IntMapElement_t const * it = intmap_cbegin(&memory); it != intmap_cend(&memory); ++it) {
        sum += it->value;
    }

    DISP(sum);
}
