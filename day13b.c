#include "common.h"
#include "vector.h"
#include "stringview.h"

inline _Bool isOdd (u64 x) {
    return !!(x&1);
}

inline _Bool isEven (u64 x) {
    return !(x&1);
}

u64 gcd (u64 a, u64 b) {
    if (!a) {
        return b;
    }

    if (!b) {
        return a;
    }

    u64 i = 0;
    while (isEven(a)) {
        a = a >> 1;
        ++i;
    }

    u64 j = 0;
    while (isEven(b)) {
        b = b >> 1;
        ++j;
    }

    u64 k = MIN(i, j);

    do {
        if (a < b) {
            u64 tmp = b;
            b = a;
            a = tmp;
        }

        a -= b;

        if (!a) {
            return (b << k);
        }

        do {
            a = a >> 1;
        } while (isEven(a));
    } while (1);
}

u64 lcm (u64 a, u64 b) {
    u64 d = gcd(a, b);
    return (a/d)*b;
}

int main (int argc, char ** argv) {
    Vector_t buses;
    vector_init(&buses, 0);

    Vector_t inds;
    vector_init(&inds, 0);

    size_t arrival;
    int ret = scanf("%zu\n", &arrival);
    ASSERT(ret == 1);

    size_t num = 0;
    do {
        char buf [BUFSIZ];
        char * s = fgets(buf, sizeof(buf), stdin);
        if (s == NULL) {
            break;
        }

        StringView_t line = sv_view_c_string(s);

        while (!sv_is_empty(&line)) {
            StringView_t n = sv_eat_until(&line, ',');
            n = sv_eat_until_space(&n);

            if (!sv_equals(&n, "x")) {
                long x = strtol(n.start, NULL, 10);
                vector_push_back(&buses, x);
                vector_push_back(&inds, num);
            }
            ++num;

            sv_eat_char(&line);
        }
    } while (1);

    size_t t = buses.data[0];
    size_t dt = t;
    if (argc > 1) {
        t = strtol(argv[1], NULL, 10);
        t = (t / dt) * dt;
    }
    for (size_t i = 1; i < buses.count; ++i) {
        while ((t+inds.data[i]) % buses.data[i]) {
            t += dt;
        }
        dt = lcm(dt, buses.data[i]);
    }

    DISP(t);
}