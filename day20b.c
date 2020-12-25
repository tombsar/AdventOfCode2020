#include "common.h"
#include "stringview.h"
#include "set.h"

typedef struct Tile {
    int id;
    char pixels [10*10];
} Tile_t;

void printTile (struct Tile * tile) {
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            putchar(tile->pixels[i*10+j]);
        }
        printf("\n");
    }
    printf("\n");
}

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

void flipEdge (char * edge) {
    for (int i = 0, j = 9; i < j; ++i, --j) {
        char tmp = edge[i];
        edge[i] = edge[j];
        edge[j] = tmp;
    }
}

void flipTileHorizontal (struct Tile * tile) {
    for (int i = 0; i < 10; ++i) {
        for (int j = 0, k = 9; j < k; ++j, --k) {
            char tmp = tile->pixels[i*10+j];
            tile->pixels[i*10+j] = tile->pixels[i*10+k];
            tile->pixels[i*10+k] = tmp;
        }
    }
}
void flipTileVertical (struct Tile * tile) {
    for (int i = 0; i < 10; ++i) {
        for (int j = 0, k = 9; j < k; ++j, --k) {
            char tmp = tile->pixels[i+j*10];
            tile->pixels[i+j*10] = tile->pixels[i+k*10];
            tile->pixels[i+k*10] = tmp;
        }
    }
}

void rotateTileRight (struct Tile * tile) {
    char tmp [10*10];
    memcpy(tmp, &(tile->pixels), 10*10);
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            tile->pixels[i*10+j] = tmp[(9-j)*10+i];
        }
    }
}

void rotateTileLeft (struct Tile * tile) {
    char tmp [10*10];
    memcpy(tmp, &(tile->pixels), 10*10);
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            tile->pixels[i*10+j] = tmp[j*10+(9-i)];
        }
    }
}

void rotateTile180 (struct Tile * tile) {
    char tmp [10*10];
    memcpy(tmp, &(tile->pixels), 10*10);
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            tile->pixels[i*10+j] = tmp[(9-i)*10+(9-j)];
        }
    }
}

int main (int argc, char ** argv) {
    size_t tiles_count = 0;
    struct Tile tiles [12*12];

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

    ASSERT(tiles_count == 12*12);

    Set_t corners;
    set_init(&corners, 4);

    Set_t unplaced;
    set_init(&unplaced, 144);

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
                        flipEdge(e);
                        if (!strncmp(edge, e, 10)) {
                            ++n_matches;
                        }
                    }
                }
            }
            ASSERT(n_matches == 0 || n_matches == 1);
            if (n_matches == 0) {
                n_unique += 1;
            }
        }

        set_add(&unplaced, i);
        if (n_unique == 2) {
            set_add(&corners, i);
        }
    }

    ASSERT(corners.count == 4);

    int grid [12*12];
    for (int i = 0; i < 12*12; ++i) {
        grid[i] = -1;
    }

    // NB: The third corner is used in order to match the orientation used by the puzzle setter
    grid[0] = corners.values[2];
    set_remove(&unplaced, grid[0]);
    {
        Tile_t * tile = &(tiles[grid[0]]);
        size_t counts [4];
        for (enum Side side = SIDE_TOP; side <= SIDE_LEFT; ++side) {
            char edge [10];
            getEdge(tile, side, &(edge[0]));

            size_t n_matches = 0;
            for (size_t j = 0; j < tiles_count; ++j) {
                Tile_t * other = &(tiles[j]);
                if (other == tile) {
                    continue;
                }
                for (enum Side s = SIDE_TOP; s <= SIDE_LEFT; ++s) {
                    char e [10];
                    getEdge(other, s, &(e[0]));
                    if (!strncmp(edge, e, 10)) {
                        ++n_matches;
                    } else {
                        flipEdge(e);
                        if (!strncmp(edge, e, 10)) {
                            ++n_matches;
                        }
                    }
                }
            }
            counts[side] = n_matches;
        }

        if (counts[0] == 0 && counts[3] == 0) {
            // Corner is already orientated correctly
        } else if (counts[0] == 0 && counts[1] == 0) {
            rotateTileLeft(tile);
        } else if (counts[1] == 0 && counts[2] == 0) {
            rotateTile180(tile);
        } else if (counts[2] == 0 && counts[3] == 0) {
            rotateTileRight(tile);
        } else {
            ASSERT(0);
        }

        // NB: Corner tile is flipped and rotated in order to match the orientation used by the puzzle setter
        flipTileVertical(tile);
        rotateTileRight(tile);
    }

    for (int i = 1; i < 12; ++i) {
        Tile_t * tile = &(tiles[grid[i-1]]);
        enum Side side = SIDE_RIGHT;
        char edge [10];
        getEdge(tile, side, &(edge[0]));
        int other_index = -1;
        enum Side matching_side;
        _Bool flipped;
        for (size_t j = 0; j < unplaced.count; ++j) {
            Tile_t * other = &(tiles[unplaced.values[j]]);
            for (enum Side s = SIDE_TOP; s <= SIDE_LEFT; ++s) {
                char e [10];
                getEdge(other, s, &(e[0]));
                if (!strncmp(edge, e, 10)) {
                    other_index = unplaced.values[j];
                    matching_side = s;
                    flipped = 0;
                    break;
                } else {
                    flipEdge(e);
                    if (!strncmp(edge, e, 10)) {
                        other_index = unplaced.values[j];
                        matching_side = s;
                        flipped = 1;
                        break;
                    }
                }
            }
            if (other_index != -1) {
                break;
            }
        }
        ASSERT(other_index != -1);
        Tile_t * other = &(tiles[other_index]);

        if (matching_side == SIDE_LEFT) {
            // Tile is already oriented correctly
        } else if (matching_side == SIDE_TOP) {
            rotateTileLeft(other);
        } else if (matching_side == SIDE_RIGHT) {
            rotateTile180(other);
        } else if (matching_side == SIDE_BOTTOM) {
            rotateTileRight(other);
        } else {
            ASSERT(0);
        }
        if (!flipped) {
            flipTileVertical(other);
        }

        grid[i] = other_index;
        set_remove(&unplaced, other_index);
    }

    for (int i = 12; i < 12*12; ++i) {
        Tile_t * tile = &(tiles[grid[i-12]]);
        enum Side side = SIDE_BOTTOM;
        char edge [10];
        getEdge(tile, side, &(edge[0]));
        int other_index = -1;
        enum Side matching_side;
        _Bool flipped;
        for (size_t j = 0; j < unplaced.count; ++j) {
            Tile_t * other = &(tiles[unplaced.values[j]]);
            for (enum Side s = SIDE_TOP; s <= SIDE_LEFT; ++s) {
                char e [10];
                getEdge(other, s, &(e[0]));
                if (!strncmp(edge, e, 10)) {
                    other_index = unplaced.values[j];
                    matching_side = s;
                    flipped = 0;
                    break;
                } else {
                    flipEdge(e);
                    if (!strncmp(edge, e, 10)) {
                        other_index = unplaced.values[j];
                        matching_side = s;
                        flipped = 1;
                        break;
                    }
                }
            }
            if (other_index != -1) {
                break;
            }
        }
        ASSERT(other_index != -1);
        Tile_t * other = &(tiles[other_index]);

        if (matching_side == SIDE_LEFT) {
            rotateTileRight(other);
        } else if (matching_side == SIDE_TOP) {
            // Tile is already oriented correctly
        } else if (matching_side == SIDE_RIGHT) {
            rotateTileLeft(other);
        } else if (matching_side == SIDE_BOTTOM) {
            rotateTile180(other);
        } else {
            ASSERT(0);
        }
        if (!flipped) {
            flipTileHorizontal(other);
        }

        grid[i] = other_index;
        set_remove(&unplaced, other_index);
    }

    ASSERT(unplaced.count == 0);

    char image [8*8*12*12];
    for (int i = 0; i < 12; ++i) {
        for (int j = 0; j < 12; ++j) {
            Tile_t * tile = &(tiles[grid[i*12+j]]);
            for (int k = 0; k < 8; ++k) {
                for (int l = 0; l < 8; ++l) {
                    image[(i*8+k)*8*12+j*8+l] = tile->pixels[(k+1)*10+(l+1)];
                }
            }
        }
    }

    size_t n_monsters = 0;
    for (int i = 0; i < 8*12-2; ++i) {
        for (int j = 0; j < 8*12-19; ++j) {
            int I = i*8*12+j;
            if (image[I+0*8*12+18] == '#' &&
                image[I+1*8*12+ 0] == '#' &&
                image[I+1*8*12+ 5] == '#' &&
                image[I+1*8*12+ 6] == '#' &&
                image[I+1*8*12+11] == '#' &&
                image[I+1*8*12+12] == '#' &&
                image[I+1*8*12+17] == '#' &&
                image[I+1*8*12+18] == '#' &&
                image[I+1*8*12+19] == '#' &&
                image[I+2*8*12+ 1] == '#' &&
                image[I+2*8*12+ 4] == '#' &&
                image[I+2*8*12+ 7] == '#' &&
                image[I+2*8*12+10] == '#' &&
                image[I+2*8*12+13] == '#' &&
                image[I+2*8*12+16] == '#')
            {
                image[I+0*8*12+18] = 'O';
                image[I+1*8*12+ 0] = 'O';
                image[I+1*8*12+ 5] = 'O';
                image[I+1*8*12+ 6] = 'O';
                image[I+1*8*12+11] = 'O';
                image[I+1*8*12+12] = 'O';
                image[I+1*8*12+17] = 'O';
                image[I+1*8*12+18] = 'O';
                image[I+1*8*12+19] = 'O';
                image[I+2*8*12+ 1] = 'O';
                image[I+2*8*12+ 4] = 'O';
                image[I+2*8*12+ 7] = 'O';
                image[I+2*8*12+10] = 'O';
                image[I+2*8*12+13] = 'O';
                image[I+2*8*12+16] = 'O';
                n_monsters += 1;
            }
        }
    }
    //DISP(n_monsters);

    size_t answer = 0;
    for (size_t i = 0; i < ARRAYCOUNT(image); ++i) {
        if (image[i] == '#') {
            answer += 1;
        }
    }
    DISP(answer);
}
