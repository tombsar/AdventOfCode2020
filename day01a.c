#include "common.h"
#include "vector.h"

int main (int argc, char ** argv) {
    Vector_t v;
    vector_init(&v, 256);
    do {
        int in;
        int ret = scanf("%i", &in);
        if (ret == 1) {
            vector_push_back(&v, in);
        } else if (ret == EOF) {
            break;
        } else {
            ERROR("scanf failed");
        }
    } while (1);

    for (size_t i = 0; i < (v.count-2); ++i) {
        int target = 2020 - (int)(v.data[i]);
        for (size_t j = i+1; j < v.count; ++j) {
            if ((int)v.data[j] == target) {
                printf("Answer: %zi\n", v.data[i]*v.data[j]);
                exit(EXIT_SUCCESS);
            }
        }
    }
}
