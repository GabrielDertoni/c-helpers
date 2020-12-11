#ifndef __TEST_UTILS_H__
#define __TEST_UTILS_H__

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <colors.h>

#define CHECKPOINT ({ last_line = __LINE__; last_fname = __FILE__; })

#define assert_eq(a, b) ({ \
    CHECKPOINT; \
    if (a != b) { \
        fprintf(stderr, YELLOW "Expected " #a " to equal " #b ". (%s:%d)" RESET "\n", __FILE__, __LINE__); \
        return false; \
    } \
})

#define assert_neq(a, b) ({ \
    CHECKPOINT; \
    if (a == b) { \
        fprintf(stderr, YELLOW "Expected " #a " to differ from " #b ". (%s:%d)" RESET "\n", __FILE__, __LINE__); \
        return false; \
    } \
})

#define assert_le(a, b) ({ \
    CHECKPOINT; \
    if (a >= b) { \
        fprintf(stderr, YELLOW "Expected " #a " to be less then " #b ". (%s:%d)" RESET "\n", __FILE__, __LINE__); \
        return false; \
    } \
})

#define assert_leq(a, b) ({ \
    CHECKPOINT; \
    if (a > b) { \
        fprintf(stderr, YELLOW "Expected " #a " to be less then or equal to " #b ". (%s:%d)" RESET "\n", __FILE__, __LINE__); \
        return false; \
    } \
})

#define assert_ge(a, b) ({ \
    CHECKPOINT; \
    if (a <= b) { \
        fprintf(stderr, YELLOW "Expected " #a " to be greater then " #b ". (%s:%d)" RESET "\n", __FILE__, __LINE__); \
        return false; \
    } \
})

#define assert_geq(a, b) ({ \
    CHECKPOINT; \
    if (a < b) { \
        fprintf(stderr, YELLOW "Expected " #a " to be greater then or equal to " #b ". (%s:%d)" RESET "\n", __FILE__, __LINE__); \
        return false; \
    } \
})

#define test_fn(func) ({ \
    CHECKPOINT; \
    if (!func) { \
        fprintf(stderr, RED "Test: " #func " failed." RESET "\n"); \
        TEST_TEARDOWN(); \
        exit(0); \
    } else \
        fprintf(stdout, GREEN "Test: " #func " succeeded." RESET "\n"); \
})

#define bench(func) ({ \
    clock_t __stime = clock(); \
    func; \
    printf(CYAN #func " took %lf milliseconds" RESET "\n", (double)(clock() - __stime) / (double)(CLOCKS_PER_SEC / 1000)); \
})

#define TEST_SETUP() ({ CHECKPOINT; test_setup(); })
#define TEST_TEARDOWN() ({ CHECKPOINT; test_teardown(); })

#define UNIMPLEMENTED() ({ printf(YELLOW "Missing implementation on (%s:%d)" RESET "\n", __FILE__, __LINE__); return true; })

#define PRINT_VEC(vec, n, flag) ({ \
    __typeof__ (n) __n = (n); \
    if (__n == 0) printf("["); \
    else printf("[ " flag, vec[0]); \
    for (int __i = 1; __i < __n; __i++) printf(", " flag, vec[__i]); \
    printf(" ]\n"); \
})

static int last_line = 0;
static char *last_fname = NULL;

void segfault_handler(int signal) {
    printf("Segfault incoming! Last checkpoint was at (%s:%d)\n",
           last_fname ? last_fname : "none", last_line);
    exit(signal);
}

void test_setup() {
    signal(SIGSEGV, segfault_handler);
    printf("\n" BLUE "========== TEST: %s ==========" RESET "\n", last_fname);
}

void test_teardown() {
    /* Nothing yet, but something could be placed here */
}

#endif
