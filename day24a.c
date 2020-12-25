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
    Set_t tiles;
    set_init(&tiles, 512);

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

        if (set_contains(&tiles, coordval)) {
            set_remove(&tiles, coordval);
        } else {
            set_add(&tiles, coordval);
        }
    } while (1);

    DISP(tiles.count);
}
