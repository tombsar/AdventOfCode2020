#include "common.h"

int main (int argc, char ** argv) {
    size_t id_max = 0;
    do {
        char buf [1024];
        char * s = fgets(buf, sizeof(buf), stdin);
        if (s == NULL) {
            break;
        }

        ASSERT(strlen(s) == 11);

        size_t row = 0;
        for (int i=0; i<7; ++i) {
            row = row << 1 | (s[i] == 'B');
        }

        size_t col = 0;
        for (int i=7; i<10; ++i) {
            col = col << 1 | (s[i] == 'R');
        }

        size_t id = row*8+col;

        id_max = MAX(id_max, id);
    } while (1);

    DISP(id_max, "%zu");
}
