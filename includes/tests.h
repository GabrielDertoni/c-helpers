#ifndef __TEST_H__
#define __TEST_H__

#include <colors.h>

#define TEST(testFn) ({ \
    printf(YELLOW "Testing: " #testFn RESET "\n"); \
    testFn; \
})

#define NAMED_TEST(name, testFn) ({ \
    printf(YELLOW "Testing: %s" RESET "\n", name); \
    testFn; \
})

#define TEST_SETUP() printf(BLUE "=============== TEST: %s ===============" RESET "\n", __FILE__)
#define TEST_TEARDOWN() printf(GREEN "%s: ALL TESTS PASSED!" RESET "\n", __FILE__)

#endif
