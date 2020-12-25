#include "common.h"

#define MAGIC UINT32_C(20201227)

u32 transform (u32 subject, u32 loop_size) {
    u64 value = UINT64_C(1);
    while (loop_size--) {
        value = (value * (u64)subject) % MAGIC;
    }
    return (u32)value;
}

int main (int argc, char ** argv) {
    u32 public [2];
    scanf("%u\n", &(public[0]));
    scanf("%u\n", &(public[1]));

    u32 private [2];

    for (int i = 0; i < 2; ++i) {
        private[i] = 0;
        u64 value = UINT32_C(1);
        do {
            private[i] += 1;
            value = (value * UINT64_C(7)) % MAGIC;
        } while ((u32)value != public[i]);
    }

    ASSERT(transform(7, private[0]) == public[0]);
    ASSERT(transform(7, private[1]) == public[1]);

    u32 key = transform(public[0], private[1]);
    DISP(key);
}
