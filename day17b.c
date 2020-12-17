#include "common.h"
#include "set.h"

typedef struct Space {
    IntHashSet_t values;
} Space_t;

s64 coordEncode (s16 x, s16 y, s16 z, s16 w) {
    union {
        s16 coords [4];
        s64 value;
    } coordunion;

    coordunion.coords[0] = x;
    coordunion.coords[1] = y;
    coordunion.coords[2] = z;
    coordunion.coords[3] = w;

    return coordunion.value;
}

void coordDecode (s64 v, s16 * x, s16 * y, s16 * z, s16 * w) {
    union {
        s16 coords [4];
        s64 value;
    } coordunion;

    coordunion.value = v;

    *x = coordunion.coords[0];
    *y = coordunion.coords[1];
    *z = coordunion.coords[2];
    *w = coordunion.coords[3];
}

int main (int argc, char ** argv) {
    IntHashSet_t space;
    inthashset_init(&space, 1024, 0);

    s16 xmin = INT16_MAX;
    s16 xmax = INT16_MIN;
    s16 ymin = INT16_MAX;
    s16 ymax = INT16_MIN;
    s16 zmin = INT16_MAX;
    s16 zmax = INT16_MIN;
    s16 wmin = INT16_MAX;
    s16 wmax = INT16_MIN;

    for (s16 y = 0; y < 8; ++y) {
        char buf [BUFSIZ];
        char * s = fgets(&(buf[0]), sizeof(buf), stdin);
        if (!s) {
            break;
        }
        ASSERT(strlen(s) > 8);
        for (s16 x = 0; x < 8; ++x) {
            if (s[x] == '#') {
                inthashset_add(&space, coordEncode(x, y, 0, 0));
                xmin = MIN(xmin, x);
                xmax = MAX(xmax, x);
                ymin = MIN(ymin, y);
                ymax = MAX(ymax, y);
                zmin = MIN(zmin, 0);
                zmax = MAX(zmax, 0);
                wmin = MIN(wmin, 0);
                wmax = MAX(wmax, 0);
            }
        }
    }

#if 0
    printf("Before any cycles:\n");
    for (s16 w = wmin; w <= wmax; ++w) {
        for (s16 z = zmin; z <= zmax; ++z) {
            printf("z=%i, w=%i\n", z, w);
            for (s16 y = ymin; y <= ymax; ++y) {
                for (s16 x = xmin; x <= xmax; ++x) {
                    if (inthashset_contains(&space, coordEncode(x, y, z, w))) {
                        printf("#");
                    } else {
                        printf(".");
                    }
                }
                printf("\n");
            }
            printf("\n");
        }
    }
    printf("\n");
#endif

    for (int cycle = 0; cycle < 6; ++cycle) {
        IntHashSet_t space_next;
        inthashset_init(&space_next, space.n_bins, 0);
        s16 xmin_next = INT16_MAX;
        s16 xmax_next = INT16_MIN;
        s16 ymin_next = INT16_MAX;
        s16 ymax_next = INT16_MIN;
        s16 zmin_next = INT16_MAX;
        s16 zmax_next = INT16_MIN;
        s16 wmin_next = INT16_MAX;
        s16 wmax_next = INT16_MIN;

        for (s16 w = wmin-1; w <= wmax+1; ++w) {
            for (s16 z = zmin-1; z <= zmax+1; ++z) {
                for (s16 y = ymin-1; y <= ymax+1; ++y) {
                    for (s16 x = xmin-1; x <= xmax+1; ++x) {
                        s16 const dxyzw [80][4] = {
                            { -1, -1, -1, -1 },
                            {  0, -1, -1, -1 },
                            {  1, -1, -1, -1 },
                            { -1,  0, -1, -1 },
                            {  0,  0, -1, -1 },
                            {  1,  0, -1, -1 },
                            { -1,  1, -1, -1 },
                            {  0,  1, -1, -1 },
                            {  1,  1, -1, -1 },
                            { -1, -1,  0, -1 },
                            {  0, -1,  0, -1 },
                            {  1, -1,  0, -1 },
                            { -1,  0,  0, -1 },
                            {  0,  0,  0, -1 },
                            {  1,  0,  0, -1 },
                            { -1,  1,  0, -1 },
                            {  0,  1,  0, -1 },
                            {  1,  1,  0, -1 },
                            { -1, -1,  1, -1 },
                            {  0, -1,  1, -1 },
                            {  1, -1,  1, -1 },
                            { -1,  0,  1, -1 },
                            {  0,  0,  1, -1 },
                            {  1,  0,  1, -1 },
                            { -1,  1,  1, -1 },
                            {  0,  1,  1, -1 },
                            {  1,  1,  1, -1 },
                            { -1, -1, -1,  0 },
                            {  0, -1, -1,  0 },
                            {  1, -1, -1,  0 },
                            { -1,  0, -1,  0 },
                            {  0,  0, -1,  0 },
                            {  1,  0, -1,  0 },
                            { -1,  1, -1,  0 },
                            {  0,  1, -1,  0 },
                            {  1,  1, -1,  0 },
                            { -1, -1,  0,  0 },
                            {  0, -1,  0,  0 },
                            {  1, -1,  0,  0 },
                            { -1,  0,  0,  0 },
                            //{  0,  0,  0,  0 }, SELF
                            {  1,  0,  0,  0 },
                            { -1,  1,  0,  0 },
                            {  0,  1,  0,  0 },
                            {  1,  1,  0,  0 },
                            { -1, -1,  1,  0 },
                            {  0, -1,  1,  0 },
                            {  1, -1,  1,  0 },
                            { -1,  0,  1,  0 },
                            {  0,  0,  1,  0 },
                            {  1,  0,  1,  0 },
                            { -1,  1,  1,  0 },
                            {  0,  1,  1,  0 },
                            {  1,  1,  1,  0 },
                            { -1, -1, -1,  1 },
                            {  0, -1, -1,  1 },
                            {  1, -1, -1,  1 },
                            { -1,  0, -1,  1 },
                            {  0,  0, -1,  1 },
                            {  1,  0, -1,  1 },
                            { -1,  1, -1,  1 },
                            {  0,  1, -1,  1 },
                            {  1,  1, -1,  1 },
                            { -1, -1,  0,  1 },
                            {  0, -1,  0,  1 },
                            {  1, -1,  0,  1 },
                            { -1,  0,  0,  1 },
                            {  0,  0,  0,  1 },
                            {  1,  0,  0,  1 },
                            { -1,  1,  0,  1 },
                            {  0,  1,  0,  1 },
                            {  1,  1,  0,  1 },
                            { -1, -1,  1,  1 },
                            {  0, -1,  1,  1 },
                            {  1, -1,  1,  1 },
                            { -1,  0,  1,  1 },
                            {  0,  0,  1,  1 },
                            {  1,  0,  1,  1 },
                            { -1,  1,  1,  1 },
                            {  0,  1,  1,  1 },
                            {  1,  1,  1,  1 }
                        };
                        size_t n_neighbours = 0;
                        for (size_t i = 0; i < ARRAYCOUNT(dxyzw); ++i) {
                            s64 in = coordEncode(x+dxyzw[i][0], y+dxyzw[i][1], z+dxyzw[i][2], w+dxyzw[i][3]);
                            if (inthashset_contains(&space, in)) {
                                n_neighbours += 1;
                            }
                        }
                        if (inthashset_contains(&space, coordEncode(x, y, z, w))) {
                            if (n_neighbours == 2 || n_neighbours == 3) {
                                inthashset_add(&space_next, coordEncode(x, y, z, w));
                                xmin_next = MIN(xmin_next, x);
                                xmax_next = MAX(xmax_next, x);
                                ymin_next = MIN(ymin_next, y);
                                ymax_next = MAX(ymax_next, y);
                                zmin_next = MIN(zmin_next, z);
                                zmax_next = MAX(zmax_next, z);
                                wmin_next = MIN(wmin_next, w);
                                wmax_next = MAX(wmax_next, w);
                            }
                        } else {
                            if (n_neighbours == 3) {
                                inthashset_add(&space_next, coordEncode(x, y, z, w));
                                xmin_next = MIN(xmin_next, x);
                                xmax_next = MAX(xmax_next, x);
                                ymin_next = MIN(ymin_next, y);
                                ymax_next = MAX(ymax_next, y);
                                zmin_next = MIN(zmin_next, z);
                                zmax_next = MAX(zmax_next, z);
                                wmin_next = MIN(wmin_next, w);
                                wmax_next = MAX(wmax_next, w);
                            }
                        }
                    }
                }
            }
        }

        inthashset_free(&space);
        inthashset_move(&space, &space_next);
        xmin = xmin_next;
        xmax = xmax_next;
        ymin = ymin_next;
        ymax = ymax_next;
        zmin = zmin_next;
        zmax = zmax_next;
        wmin = wmin_next;
        wmax = wmax_next;

#if 0
        printf("After %i cycle%s:\n\n", cycle+1, cycle+1==1?"":"s");
        for (s16 w = wmin; w <= wmax; ++w) {
            for (s16 z = zmin; z <= zmax; ++z) {
                printf("z=%i, w=%i\n", z, w);
                for (s16 y = ymin; y <= ymax; ++y) {
                    for (s16 x = xmin; x <= xmax; ++x) {
                        if (inthashset_contains(&space, coordEncode(x, y, z, w))) {
                            printf("#");
                        } else {
                            printf(".");
                        }
                    }
                    printf("\n");
                }
                printf("\n");
            }
        }
        printf("\n");
#endif
    }

    DISP(inthashset_count(&space));
}
