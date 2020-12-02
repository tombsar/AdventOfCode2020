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

    for (size_t i = 0; i < (v.count-3); ++i) {
        for (size_t j = i+1; j < (v.count-2); ++j) {
            int target = 2020 - v.data[i] - v.data[j];
            for (size_t k = j+1; k < v.count; ++k) {
                if (v.data[k] == target) {
                    printf("Answer: %i\n", v.data[i]*v.data[j]*v.data[k]);
                    exit(EXIT_SUCCESS);
                }
            }
        }
    }
}
