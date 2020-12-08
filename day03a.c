#include "common.h"

typedef struct TreeGrid {
    char * grid;
    size_t width, height;
} TreeGrid_t;

void treegrid_resize (struct TreeGrid * grid, size_t w, size_t h) {
    grid->grid = realloc(grid->grid, w*h);
    grid->width = w;
    grid->height = h;
}

void treegrid_push_row (struct TreeGrid * grid, char const * data) {
    treegrid_resize(grid, grid->width, grid->height+1);
    memcpy(&(grid->grid[(grid->height-1)*grid->width]), data, grid->width);
}

char treegrid_at (struct TreeGrid const * grid, size_t x, size_t y) {
    ASSERT(y < grid->height);
    return grid->grid[y*grid->width + (x % grid->width)];
}

size_t toboggan (struct TreeGrid const * grid, size_t dx, size_t dy) {
    size_t n = 0;
    for (size_t x = 0, y = 0; y < grid->height; x += dx, y += dy) {
        if (treegrid_at(grid, x, y) == '#') {
            ++n;
        }
    }
    return n;
}

int main (int argc, char ** argv) {
    TreeGrid_t grid = {};

    char buf [256];
    // HACK: Vulnerable to buffer overflow!
    scanf("%s", &(buf[0]));

    grid.width = strlen(&(buf[0]));

    treegrid_push_row(&grid, buf);

    do {
        // HACK: Vulnerable to buffer overflow!
        int ret = scanf("%s", &(buf[0]));
        if (ret == 1) {
            treegrid_push_row(&grid, buf);
        } else if (ret == EOF) {
            break;
        } else {
            ERROR("scanf failed");
        };
    } while (1);

    size_t n_trees = toboggan(&grid, 3, 1);

    DISP(n_trees);
}
