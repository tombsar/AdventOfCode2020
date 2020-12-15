#include "common.h"
#include "vector.h"
#include "map.h"

size_t const N_ROUNDS = 30000000;

int main (int argc, char ** argv) {
    Vector_t nums;
    vector_init(&nums, N_ROUNDS);

    IntHashMap_t last;
    inthashmap_init(&last, 1024, N_ROUNDS/1024);

    do {
        long int x;
        int ret = scanf("%li", &x);
        if (ret == 1) {
            inthashmap_set(&last, x, nums.count);
            vector_push_back(&nums, x);
        } else if (ret == EOF) {
            break;
        } else {
            ERROR("scanf failed");
        }
    } while (1);

    intptr_t x_index = -1;
    while (nums.count < N_ROUNDS) {
        long int y = 0;
        if (x_index != -1) {
            y = nums.count - 1 - x_index;
        }
        if (inthashmap_contains(&last, y)) {
            x_index = inthashmap_get(&last, y);
        } else {
            x_index = -1;
        }
        inthashmap_set(&last, y, nums.count);
        vector_push_back(&nums, y);
    }
    DISP(nums.data[nums.count-1]);
}
