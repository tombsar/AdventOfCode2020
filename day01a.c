#include "common.h"
#include "vector.h"

int main (int argc, char ** argv) {
    Vector_t v;
    vector_init(&v, 256);
    do {
        int in;
        int ret = scanf("%i", &in);
        if (ret == 1) {
            vector_insert_sorted(&v, in);
        } else if (ret == EOF) {
            break;
        } else {
            ERROR("scanf failed");
        }
    } while (1);

    for (size_t i = 0; i < v.count; ++i) {
        int target = 2020 - (int)(v.data[i]);
        if (vector_find_sorted(&v, target) != -1) {
            printf("Answer: %zi\n", v.data[i]*target);
            return 0;
        }
    }
}
