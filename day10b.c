#include "common.h"
#include "set.h"

size_t countValidCombinations (intptr_t jolts_now, intptr_t jolts_target, intptr_t const * start, intptr_t const * end, size_t * memo) {
    if (memo[jolts_now]) {
        return memo[jolts_now];
    }

    size_t n = 0;

    if (jolts_now == jolts_target) {
        n = 1;
    } else {
        while (start != end && (start[0]-jolts_now) < 4) {
            n += countValidCombinations(start[0], jolts_target, &(start[1]), end, memo);
            ++start;
        }
    }

    memo[jolts_now] = n;
    return n;
}

int main (int argc, char ** argv) {
    Set_t adapters;
    set_init(&adapters, 128);
    do {
        intptr_t in;
        int ret = scanf("%zi", &in);
        if (ret == 1) {
            set_add(&adapters, in);
        } else if (ret == EOF) {
            break;
        } else {
            ERROR("scanf failed");
        }
    } while (1);

    //intptr_t adapter_min = adapters.data[0];
    intptr_t adapter_max = set_max(&adapters);

    size_t * memo = calloc(adapter_max, sizeof(size_t));
    size_t combinations = countValidCombinations(0, adapter_max, set_begin(&adapters), set_end(&adapters), memo);
    DISP(combinations);
}
