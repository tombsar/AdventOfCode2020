#include "common.h"
#include "vector.h"
#include "map.h"

size_t const N_ROUNDS = 30000000;

int main (int argc, char ** argv) {
    Vector_t nums;
    vector_init(&nums, N_ROUNDS);

    IntMap_t last;
    intmap_init(&last, N_ROUNDS);

    do {
        long int x;
        int ret = scanf("%li", &x);
        if (ret == 1) {
            intmap_set(&last, x, nums.count);
            vector_push_back(&nums, x);
        } else if (ret == EOF) {
            break;
        } else {
            ERROR("scanf failed");
        }
    } while (1);

    ptrdiff_t x_index = -1;
    while (nums.count < N_ROUNDS) {
        long int y = 0;
        if (x_index != -1) {
            y = nums.count - 1 - x_index;
        }
        ptrdiff_t ind = intmap_find(&last, y);
        if (ind != -1) {
            x_index = last.values[ind];
            last.values[ind] = nums.count;
        } else {
            x_index = -1;
            intmap_set(&last, y, nums.count);
        }
        vector_push_back(&nums, y);
    }
    DISP(nums.data[nums.count-1]);
}
