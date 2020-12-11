#include "common.h"

typedef struct SeatGrid {
    char * grid;
    size_t n_rows, n_cols;
} SeatGrid_t;

void seatgrid_resize (struct SeatGrid * grid, size_t n_rows, size_t n_cols) {
    grid->grid = realloc(grid->grid, n_rows*n_cols);
    grid->n_rows = n_rows;
    grid->n_cols = n_cols;
}

void seatgrid_copy (struct SeatGrid * dest, struct SeatGrid const * src) {
    seatgrid_resize(dest, src->n_rows, src->n_cols);
    memcpy(dest->grid, src->grid, src->n_rows * src->n_cols);
}

void seatgrid_free (struct SeatGrid * grid) {
    free(grid->grid);
    grid->n_rows = 0;
    grid->n_cols = 0;
    grid->grid = NULL;
}

void seatgrid_push_row (struct SeatGrid * grid, char const * data) {
    seatgrid_resize(grid, grid->n_rows+1, grid->n_cols);
    memcpy(&(grid->grid[(grid->n_rows-1)*grid->n_cols]), data, grid->n_cols);
}

char seatgrid_get (struct SeatGrid const * grid, size_t r, size_t c) {
    ASSERT(r < grid->n_rows && c < grid->n_cols);
    return grid->grid[r*grid->n_cols + c];
}

void seatgrid_set (struct SeatGrid * grid, size_t r, size_t c, char s) {
    ASSERT(r < grid->n_rows && c < grid->n_cols);
    grid->grid[r*grid->n_cols + c] = s;
}

void seatgrid_print (struct SeatGrid const * grid) {
    for (size_t r = 0; r < grid->n_rows; ++r) {
        for (size_t c = 0; c < grid->n_cols; ++c) {
            printf("%c", seatgrid_get(grid, r, c));
        }
        printf("\n");
    }
}

_Bool isOccupied (struct SeatGrid const * grid, intptr_t r, intptr_t c) {
    if (r < 0 || (size_t)r >= grid->n_rows || c < 0 || (size_t)c >= grid->n_cols) {
        return 0;
    }
    return (seatgrid_get(grid, (size_t)r, (size_t)c) == '#');
}

size_t countOccupied (struct SeatGrid const * grid) {
    size_t n = 0;
    for (size_t r = 0; r < grid->n_rows; ++r) {
        for (size_t c = 0; c < grid->n_cols; ++c) {
            if (isOccupied(grid, r, c)) {
                ++n;
            }
        }
    }
    return n;
}

size_t countNeighbours (struct SeatGrid const * grid, size_t r, size_t c) {
    size_t n = 0;
    n += isOccupied(grid, (intptr_t)r-1, (intptr_t)c-1);
    n += isOccupied(grid, (intptr_t)r-1, (intptr_t)c);
    n += isOccupied(grid, (intptr_t)r-1, (intptr_t)c+1);
    n += isOccupied(grid, (intptr_t)r,   (intptr_t)c-1);
    n += isOccupied(grid, (intptr_t)r,   (intptr_t)c+1);
    n += isOccupied(grid, (intptr_t)r+1, (intptr_t)c-1);
    n += isOccupied(grid, (intptr_t)r+1, (intptr_t)c);
    n += isOccupied(grid, (intptr_t)r+1, (intptr_t)c+1);
    return n;
}

size_t simulate (struct SeatGrid * state) {
    struct SeatGrid next = {};
    seatgrid_copy(&next, state);

    size_t n_changes = 0;
    for (size_t r = 0; r < state->n_rows; ++r) {
        for (size_t c = 0; c < state->n_cols; ++c) {
            char s = seatgrid_get(state, r, c);
            switch (s) {
                case 'L': {
                    if (countNeighbours(state, r, c) == 0) {
                        seatgrid_set(&next, r, c, '#');
                        ++n_changes;
                    }
                } break;
                case '#': {
                    if (countNeighbours(state, r, c) >= 4) {
                        seatgrid_set(&next, r, c, 'L');
                        ++n_changes;
                    }
                } break;
            }
        }
    }

    seatgrid_copy(state, &next);
    seatgrid_free(&next);

    return n_changes;
}

int main (int argc, char ** argv) {
    SeatGrid_t grid = {};

    char buf [256];
    scanf("%255s", &(buf[0]));

    grid.n_cols = strlen(&(buf[0]));

    seatgrid_push_row(&grid, buf);

    do {
        int ret = scanf("%255s", &(buf[0]));
        if (ret == 1) {
            seatgrid_push_row(&grid, buf);
        } else if (ret == EOF) {
            break;
        } else {
            ERROR("scanf failed");
        };
    } while (1);

    size_t n_changes;
    do {
        n_changes = simulate(&grid);
        //seatgrid_print(&grid);
        //printf("\n");
    } while (n_changes);

    size_t n_occupied = countOccupied(&grid);
    DISP(n_occupied);
}
