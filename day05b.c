#include "common.h"

int main (int argc, char ** argv) {
    _Bool ids [128*8] = {};

    do {
        char buf [BUFSIZ];
        char * s = fgets(buf, sizeof(buf), stdin);
        if (s == NULL) {
            break;
        }

        ASSERT(strlen(s) == 11);

        size_t id = 0;
        for (int i=0; i<7; ++i) {
            ASSERT(s[i] == 'F' || s[i] == 'B');
            id = (id << 1) | (s[i] == 'B');
        }

        for (int i=7; i<10; ++i) {
            ASSERT(s[i] == 'L' || s[i] == 'R');
            id = (id << 1) | (s[i] == 'R');
        }

        ids[id] = 1;
    } while (1);

    size_t i = 0;
    while (!ids[i]) {
        ++i;
        ASSERT(i < 128*8);
    }
    while (ids[i]) {
        ++i;
        ASSERT(i < 128*8);
    }
    DISP(i, "%zu");
}
