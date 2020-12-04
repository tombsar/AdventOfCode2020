#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define WARN(MSG) do { fprintf(stderr, "\033[1m%s:\033[0m In function <\033[1m%s\033[0m>:\n\033[1m%s:%d: \033[35;1mwarning:\033[0m %s\n", __FILE__, __func__, __FILE__, __LINE__, MSG); } while(0)
#define ERROR(MSG) do { fprintf(stderr, "\033[1m%s:\033[0m In function <\033[1m%s\033[0m>:\n\033[1m%s:%d: \033[31;1merror:\033[0m %s\n", __FILE__, __func__, __FILE__, __LINE__, MSG); exit(EXIT_FAILURE); } while(0)

#ifndef NDEBUG
#define ASSERT(X) do { if (!(X)) { fprintf(stderr, "\033[1m%s:\033[0m In function <\033[1m%s\033[0m>:\n\033[1m%s:%d: \033[31;1merror:\033[0m Assertion \"%s\" failed\n", __FILE__, __func__, __FILE__, __LINE__, #X); exit(EXIT_FAILURE); } } while(0)
#else
#define ASSERT(X)
#endif

#define DISP(X,FMT) do { printf("\033[1m%s:\033[0m In function <\033[1m%s\033[0m>\n\033[1m%s:%d: \033[36mdisp:\033[0m %s = " FMT "\n", __FILE__, __func__, __FILE__, __LINE__, #X, X); } while (0)

typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#define MIN(A,B) ((A)<(B)?(A):(B))
#define MAX(A,B) ((A)>(B)?(A):(B))
#define ABS(X) ((X)>=0?(X):-(X))
