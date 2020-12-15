#pragma once

#include "common.h"

/* Reversible 32- and 64-bit integer hash functions taken from https://stackoverflow.com/a/12996028 */

u32 hash_u32 (u32 x) {
    x = ((x >> 16) ^ x) * UINT32_C(0x045d9f3b);
    x = ((x >> 16) ^ x) * UINT32_C(0x045d9f3b);
    x = ((x >> 16) ^ x);
    return x;
}

u32 unhash_u32 (u32 x) {
    x = ((x >> 16) ^ x) * UINT32_C(0x119de1f3);
    x = ((x >> 16) ^ x) * UINT32_C(0x119de1f3);
    x = ((x >> 16) ^ x);
    return x;
}

u64 hash_u64 (u64 x) {
    x = (x ^ (x >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
    x = (x ^ (x >> 27)) * UINT64_C(0x94d049bb133111eb);
    x = (x ^ (x >> 31));
    return x;
}

u64 unhash_u64 (u64 x) {
    x = (x ^ (x >> 31) ^ (x >> 62)) * UINT64_C(0x319642b2d24d8ec3);
    x = (x ^ (x >> 27) ^ (x >> 54)) * UINT64_C(0x96de1b173f119089);
    x = (x ^ (x >> 30) ^ (x >> 60));
    return x;
}

/* djb2 hash function taken from http://www.cse.yorku.ca/~oz/hash.html */
u32 hash_string (char const * start, char const * end) {
    u32 hash = 5381;
    while (start < end) {
        u32 c = *start;
        hash = ((hash << 5) + hash) + c;
        ++start;
    }
    return hash;
}

u32 hash_c_string (char const * str) {
    u32 hash = 5381;
    while (*str) {
        u32 c = *str;
        hash = ((hash << 5) + hash) + c;
        ++str;
    }
    return hash;
}

#define HASH(X) _Generic((X), s32: hash_u32, s64: hash_u64, u32: hash_u32, u64: hash_u64, char const *: hash_c_string)(X)
#define UNHASH(X) _Generic((X), s32: unhash_u32, s64: unhash_u64, u32: unhash_u32, u64: unhash_u64)(X)
