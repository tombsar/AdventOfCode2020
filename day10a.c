#include "common.h"
#include "set.h"

int main (int argc, char ** argv) {
    IntSet_t adapters;
    intset_init(&adapters, 128);
    do {
        intptr_t in;
        int ret = scanf("%zi", &in);
        if (ret == 1) {
            intset_add(&adapters, in);
        } else if (ret == EOF) {
            break;
        } else {
            ERROR("scanf failed");
        }
    } while (1);

    //intptr_t adapter_min = adapters.data[0];
    intptr_t adapter_max = intset_max(&adapters);

    intptr_t hist [3] = {};

    intptr_t last = 0;
    for (intptr_t * it = intset_begin(&adapters); it != intset_end(&adapters); ++it) {
        intptr_t diff = *it - last;
        ASSERT(diff == 1 || diff == 2 || diff == 3);
        hist[diff-1] += 1;
        last = *it;
    }
    {
        intptr_t diff = adapter_max + 3 - last;
        ASSERT(diff == 1 || diff == 2 || diff == 3);
        hist[diff-1] += 1;
    }

    DISP(hist[0]);
    DISP(hist[1]);
    DISP(hist[2]);
    DISP(hist[0]*hist[2]);
}
