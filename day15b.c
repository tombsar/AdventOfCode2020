#include "common.h"
#include "vector.h"
#include "map.h"

int main (int argc, char ** argv) {
    Vector_t nums;
    vector_init(&nums, 0);

    IntMap_t last;
    intmap_init(&last, 0);

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

    intptr_t x_index = -1;
    while (nums.count < 30000000) {
        long int y = 0;
        if (x_index != -1) {
            y = nums.count - 1 - x_index;
        }
        intptr_t ind = intmap_find(&last, y);
        if (ind != -1) {
            x_index = last.data[ind].value;
        } else {
            x_index = -1;
        }
        intmap_set(&last, y, nums.count);
        vector_push_back(&nums, y);

        if (!(nums.count % 1000000)) {
            printf("%zu...\n", nums.count);
        }
    }
    DISP(nums.data[nums.count-1]);
}
