#include "common.h"
#include "vector.h"

int main (int argc, char ** argv) {
    Vector_t numbs;
    vector_init(&numbs, 256);

    for (int i = 0; i < 25; ++i) {
        long unsigned int in;
        int ret = scanf("%lu", &in);
        ASSERT(ret == 1);
        vector_push_back(&numbs, in);
    }

    size_t invalid_num = 0;
    do {
        long unsigned int in;
        int ret = scanf("%lu", &in);
        if (ret == 1) {
            _Bool found = 0;
            for (size_t i = numbs.count-25; i < numbs.count; ++i) {
                for (size_t j = i+1; j < numbs.count; ++j) {
                    if ((long unsigned int)numbs.data[i] + (long unsigned int)numbs.data[j] == in) {
                        found = 1;
                        break;
                    }
                }
            }
            if (!found) {
                invalid_num = in;
                break;
            }
            vector_push_back(&numbs, in);
        } else if (ret == EOF) {
            break;
        } else {
            ERROR("scanf failed");
        }
    } while (1);

    Vector_t tmp;
    vector_init(&tmp, numbs.count);
    for (intptr_t i = numbs.count-1; i >= 0; --i) {
        size_t sum = 0;
        for (intptr_t j = i; j < (intptr_t)numbs.count; ++j) {
            long unsigned int x = numbs.data[j];
            vector_insert_sorted(&tmp, x);
            sum += x;
            if (sum == invalid_num) {
                intptr_t min = tmp.data[0];
                intptr_t max = tmp.data[tmp.count-1];
                DISP(min+max);
                return 0;
            } else if (sum > invalid_num) {
                break;
            }
        }
        vector_clear(&tmp);
    }
}
