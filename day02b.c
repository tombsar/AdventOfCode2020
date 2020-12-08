#include "common.h"

typedef struct Policy {
    int pos0;
    int pos1;
    char c;
} Policy_t;

_Bool validate (char const * pass, Policy_t policy) {
    int n_c = 0;
    if (pass[policy.pos0-1] == policy.c) {
        ++n_c;
    }
    if (pass[policy.pos1-1] == policy.c) {
        ++n_c;
    }
    return (n_c == 1);
}

int main (int argc, char ** argv) {
    int n_valid = 0;
    do {
        Policy_t policy;
        char password [256];
        int ret = scanf("%i-%i %c: %255s", &(policy.pos0), &(policy.pos1), &(policy.c), &(password[0]));
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
