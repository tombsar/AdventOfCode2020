#include "common.h"

int main (int argc, char ** argv) {
    size_t id_max = 0;
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

        id_max = MAX(id_max, id);
    } while (1);

    DISP(id_max, "%zu");
}
