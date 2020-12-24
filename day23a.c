#include "common.h"
#include "vector.h"

int main (int argc, char ** argv) {
    Vector_t cups;
    vector_init(&cups, 9);

    char buf [BUFSIZ];
    char * s = fgets(&(buf[0]), sizeof(buf), stdin);
    while (isdigit(*s)) {
        vector_push_back(&cups, *s-'0');
        ++s;
    }

    intptr_t minlabel = vector_min(&cups);
    intptr_t maxlabel = vector_max(&cups);

    size_t Icurrent = 0;
    for (long int move = 1; move <= 100; ++move) {
        printf("-- move %li --\ncups:", move);
        for (size_t j = 0; j < cups.count; ++j) {
            if (j == Icurrent) {
                printf(" (%zi)", cups.data[j]);
            } else {
                printf(" %zi", cups.data[j]);
            }
        }
        printf("\n");

        intptr_t removed [3];
        for (size_t i = 0; i < 3; ++i) {
            size_t ind = (Icurrent+1) % cups.count;
            removed[i] = cups.data[ind];
            vector_erase(&cups, ind);
            if (ind < Icurrent) {
                --Icurrent;
            }
        }
        printf("pick up:");
        for (size_t j = 0; j < 3; ++j) {
            printf("%s %zi", (j)?",":"", removed[j]);
        }
        printf("\n");

        intptr_t currentlabel = cups.data[Icurrent];
        intptr_t destlabel = currentlabel;
        ptrdiff_t Idest;
        do {
            destlabel -= 1;
            if (destlabel < minlabel) {
                destlabel = maxlabel;
            }
            Idest = vector_find(&cups, destlabel);
        } while (Idest < 0);
        vector_insert(&cups, removed[2], Idest+1);
        if (Idest+1 <= (ptrdiff_t)Icurrent) {
            ++Icurrent;
        }
        vector_insert(&cups, removed[1], Idest+1);
        if (Idest+1 <= (ptrdiff_t)Icurrent) {
            ++Icurrent;
        }
        vector_insert(&cups, removed[0], Idest+1);
        if (Idest+1 <= (ptrdiff_t)Icurrent) {
            ++Icurrent;
        }

        Icurrent = (Icurrent + 1) % cups.count;

        printf("destination: %zi\n", destlabel);
        printf("\n");
    }

    ptrdiff_t I1 = vector_find(&cups, 1);
    ASSERT(I1 >= 0);
    for (size_t i = 1; i < cups.count; ++i) {
        size_t ind = (I1+i) % cups.count;
        printf("%zi", cups.data[ind]);
    }
    printf("\n");
}
