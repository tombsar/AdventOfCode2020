#include "common.h"
#include "stringview.h"

typedef struct Tile {
    int id;
    char pixels [10*10];
} Tile_t;

enum Side {
    SIDE_TOP = 0,
    SIDE_RIGHT = 1,
    SIDE_BOTTOM = 2,
    SIDE_LEFT = 3
};

int edgeToInt (char * edge) {
    int v = 0;
    for (int i = 0; i < 10; ++i) {
        v = (v << 1) | (edge[i] == '#');
    }
    return v;
}

void getEdge (struct Tile * tile, enum Side side, char * out) {
    int ind = 0;
    int dind = 0;
    switch (side) {
        case SIDE_TOP: {
            ind = 0;
            dind = 1;
        } break;
        case SIDE_RIGHT: {
            ind = 9;
            dind = 10;
        } break;
        case SIDE_BOTTOM: {
            ind = 99;
            dind = -1;
        } break;
        case SIDE_LEFT: {
            ind = 90;
            dind = -10;
        } break;
    }
    for (int i = 0; i < 10; ++i) {
        *(out++) = tile->pixels[ind];
        ind += dind;
    }
}

void flip (char * edge) {
    for (int i = 0, j = 9; i < j; ++i, --j) {
        char tmp = edge[i];
        edge[i] = edge[j];
        edge[j] = tmp;
    }
}

int main (int argc, char ** argv) {
    size_t tiles_count = 0;
    struct Tile tiles [144];

    do {
        char buf [BUFSIZ];
        char * s = fgets(&(buf[0]), sizeof(buf), stdin);
        if (!s) {
            break;
        }

        StringView_t line = sv_view_c_string(s);
        sv_eat_spaces(&line);
        if (sv_is_empty(&line)) {
            continue;
        }

        StringView_t tile_str = sv_eat_until_space(&line);
        if (sv_equals(&tile_str, "Tile")) {
            sv_eat_spaces(&line);
            long id = strtol(line.start, NULL, 10);
            ASSERT(tiles_count < ARRAYCOUNT(tiles));
            long index = tiles_count++;
            struct Tile * tile = &(tiles[index]);
            tile->id = id;
            for (int i = 0; i < 10; ++i) {
                char * s = fgets(&(buf[0]), sizeof(buf), stdin);
                ASSERT(s);
                ASSERT(strlen(s) == 10+1);
                for (int j = 0; j < 10; ++j) {
                    tile->pixels[i*10+j] = *(s++);
                }
            }
        }
    } while (1);

    ASSERT(tiles_count == 144);

    size_t n_corners = 0;
    int corners [4];

    for (size_t i = 0; i < tiles_count; ++i) {
        Tile_t * tile = &(tiles[i]);
        size_t n_unique = 0;
        for (enum Side side = SIDE_TOP; side <= SIDE_LEFT; ++side) {
            char edge [10];
            getEdge(tile, side, &(edge[0]));

            size_t n_matches = 0;
            for (size_t j = 0; j < tiles_count; ++j) {
                if (i == j) {
                    continue;
                }
                Tile_t * other = &(tiles[j]);
                for (enum Side s = SIDE_TOP; s <= SIDE_LEFT; ++s) {
                    char e [10];
                    getEdge(other, s, &(e[0]));
                    if (!strncmp(edge, e, 10)) {
                        ++n_matches;
                    } else {
                        flip(e);
                        if (!strncmp(edge, e, 10)) {
                            ++n_matches;
                        }
                    }
                }
            }

            if (!n_matches) {
                ++n_unique;
            }
        }

        if (n_unique == 2) {
            ASSERT(n_corners < ARRAYCOUNT(corners));
            corners[n_corners++] = i;
        }
    }

    DISP(n_corners);

    size_t result = 1;
    for (size_t i = 0; i < n_corners; ++i) {
        result *= tiles[corners[i]].id;
    }
    DISP(result);
}
