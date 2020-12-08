#include "common.h"

typedef struct Policy {
    int n_min;
    int n_max;
    char c;
} Policy_t;

_Bool validate (char const * pass, Policy_t policy) {
    int n_c = 0;
    char const * p = pass;
    while (*p != '\0') {
        if (*p == policy.c) {
            ++n_c;
        }
        ++p;
    }
    return (n_c >= policy.n_min && n_c <= policy.n_max);
}

int main (int argc, char ** argv) {
    int n_valid = 0;
    do {
        Policy_t policy;
        char password [256];
        int ret = scanf("%i-%i %c: %255s", &(policy.n_min), &(policy.n_max), &(policy.c), &(password[0]));
        if (ret == 4) {
            if (validate(password, policy)) {
                ++n_valid;
            }
        } else if (ret == EOF) {
            break;
        } else {
            ERROR("scanf failed");
        }
    } while (1);

    DISP(n_valid);
}
