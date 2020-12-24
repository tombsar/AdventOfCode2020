#include "common.h"
#include "set.h"

s64 pack (s32 q, s32 r) {
    union {
        s32 coords [2];
        s64 packedvalue;
    } coordunion;
    coordunion.coords[0] = q;
    coordunion.coords[1] = r;
    return coordunion.packedvalue;
}

void unpack (s64 v, s32 * q, s32 * r) {
    union {
        s32 coords [2];
        s64 packedvalue;
    } coordunion;
    coordunion.packedvalue = v;
    *q = coordunion.coords[0];
    *r = coordunion.coords[1];
}

enum Direction {
    DIR_E,
    DIR_NE,
    DIR_NW,
    DIR_W,
    DIR_SW,
    DIR_SE
};

void offset (s32 * q, s32 * r, enum Direction direction, s32 distance) {
    s32 dq = 0;
    s32 dr = 0;
    switch (direction) {
        case DIR_E: {
            dq = 1;
        } break;
        case DIR_NE: {
            dr = 1;
        } break;
        case DIR_NW: {
            dq = -1;
            dr = 1;
        } break;
        case DIR_W: {
            dq = -1;
        } break;
        case DIR_SW: {
            dr = -1;
        } break;
        case DIR_SE: {
            dq = 1;
            dr = -1;
        } break;
    }
    *q += dq*distance;
    *r += dr*distance;
}

int main (int argc, char ** argv) {
    IntSet_t tiles;
    intset_init(&tiles, 512);

    do {
        char buf [BUFSIZ];
        char * s = fgets(&(buf[0]), sizeof(buf), stdin);
        if (!s) {
            break;
        }

        s32 q = 0;
        s32 r = 0;

        while (*s != '\n') {
            enum Direction dir = DIR_E;
            if (*s == 'e') {
                dir = DIR_E;
                ++s;
            } else if (*s == 'w') {
                dir = DIR_W;
                ++s;
            } else if (*s == 'n') {
                ++s;
                if (*s == 'e') {
                    dir = DIR_NE;
                } else if (*s == 'w') {
                    dir = DIR_NW;
                }
                ++s;
            } else if (*s == 's') {
                ++s;
                if (*s == 'e') {
                    dir = DIR_SE;
                } else if (*s == 'w') {
                    dir = DIR_SW;
                }
                ++s;
            } else {
                ASSERT(0);
            }
            offset(&q, &r, dir, 1);
        }

        s64 coordval = pack(q, r);

        if (intset_contains(&tiles, coordval)) {
            intset_remove(&tiles, coordval);
        } else {
            intset_add(&tiles, coordval);
        }
    } while (1);

    for (int day = 0; day < 100; ++day) {
        IntSet_t tiles_new;
        intset_copy(&tiles_new, &tiles);

        s32 qmin = INT32_MAX, qmax = INT32_MIN;
        s32 rmin = INT32_MAX, rmax = INT32_MIN;
        for (intptr_t const * it = intset_cbegin(&tiles); it != intset_cend(&tiles); ++it) {
            s32 q, r;
            unpack(*it, &q, &r);
            qmin = MIN(qmin, q);
            qmax = MAX(qmax, q);
            rmin = MIN(rmin, r);
            rmax = MAX(rmax, r);
        }

        for (s32 q = qmin-1; q < qmax+2; ++q) {
            for (s32 r = rmin-1; r < rmax+2; ++r) {
                int n_neighbours = 0;
                for (enum Direction dir = DIR_E; dir <= DIR_SE; ++dir) {
                    s32 qn = q;
                    s32 rn = r;
                    offset(&qn, &rn, dir, 1);
                    if (intset_contains(&tiles, pack(qn, rn))) {
                        ++n_neighbours;
                    }
                }
                s64 coordval = pack(q, r);
                if (intset_contains(&tiles, coordval)) {
                    if (n_neighbours == 0 || n_neighbours > 2) {
                        intset_remove(&tiles_new, coordval);
                    }
                } else {
                    if (n_neighbours == 2) {
                        intset_add(&tiles_new, coordval);
                    }
                }
            }
        }

        intset_free(&tiles);
        intset_move(&tiles, &tiles_new);
    }

    DISP(tiles.count);
}
