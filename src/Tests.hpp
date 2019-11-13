#ifndef TESTS_HPP_
#define TESTS_HPP_

#include <R.h>
#include <Rinternals.h>
#include <string>
#include <vector>
#include <functional>


#define CHECK(cond) \
    if (!(cond)) \
        Tests::error_(__FILE__, __LINE__, NULL, 0)

#define CHECK_MSG(cond, msg) \
    if (!(cond)) \
        Tests::error_(__FILE__, __LINE__, msg, 0)

#define ASSERT(cond) \
    if (!(cond)) \
        Tests::error_(__FILE__, __LINE__, NULL, 1)


struct Test {
    std::string name;
    std::function<void(void)> func;
};

class Tests {
public:
    static bool runAll(const std::vector< Test> &tests);
    static void error_(const char *file, int line, const char *msg, int fail);

    template <typename T>
    static bool checkBuffers(const T *expected, const T *actual, size_t size)
    {
        for (size_t i = 0; i < size; i++) {
            if (expected[i] != actual[i]) {
                Rprintf("Error: buffers not equal - expected=");
                printBuffer(expected, size);
                Rprintf("  , actual=");
                printBuffer(actual, size);
                Rprintf("\n");
                return false;
            }
        }
        return true;
    }

    template <typename T>
    static void printBuffer(const T *buffer, size_t size)
    {
        Rprintf("[");
        for (size_t i = 0; i < size; i++) {
            Rprintf("%d,", buffer[i]);
        }
        Rprintf("]");
    }

private:
    static const Test *curr_test;
    static std::vector< Test> failed_tests;

    static void addFailedTest();
    static void beforeRunAll();
    static void afterRunAll();
};


#endif // TESTS_HPP_
