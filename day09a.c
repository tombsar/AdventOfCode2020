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
                DISP(in);
                break;
            }
            vector_push_back(&numbs, in);
        } else if (ret == EOF) {
            break;
        } else {
            ERROR("scanf failed");
        }
    } while (1);
}
