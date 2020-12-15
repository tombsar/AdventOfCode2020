#include "common.h"
#include "vector.h"

int main (int argc, char ** argv) {
    Vector_t nums;
    vector_init(&nums, 0);

    long int x;
    do {
        int ret = scanf("%li", &x);
        if (ret == 1) {
            vector_push_back(&nums, x);
        } else if (ret == EOF) {
            break;
        } else {
            ERROR("scanf failed");
        }
    } while (1);

    while (nums.count < 2020) {
        ptrdiff_t ind = vector_find(&nums, x);
        int y = 0;
        if (ind != -1) {
            for (intptr_t i = nums.count-2; i >= ind; --i) {
                if (nums.data[i] == x) {
                    y = nums.count - 1 - i;
                    break;
                }
            }
        }
        vector_push_back(&nums, y);
        x = y;
    }
    DISP(nums.data[nums.count-1]);
}
