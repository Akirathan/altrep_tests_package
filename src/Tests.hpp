#ifndef TESTS_HPP_
#define TESTS_HPP_

#include <R.h>
#include <Rinternals.h>
// TODO: This is a quickfix for clang - there is apperently a macro clash in <sstream> with length macro.
#undef length
#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <sstream>
#define length Rf_length


#define CHECK(cond) \
    if (!(cond)) \
        Tests::error_(__FILE__, __LINE__, NULL, 0)

#define CHECK_MSG(cond, msg) \
    if (!(cond)) \
        Tests::error_(__FILE__, __LINE__, msg, 0)

#define ASSERT(cond) \
    if (!(cond)) \
        Tests::error_(__FILE__, __LINE__, NULL, 1)

#define SKIP_IF_NOT(cond) \
    if (!(cond)) { \
        Rprintf("Some test precondition (at %s:%d) is not satisfied, skipping <func_name>\n", __FILE__, __LINE__); \
        return; \
    }


struct Test {
    std::string name;
    std::function<void(void)> func;
};

class Tests {
public:
    static bool runAll(const std::vector< Test> &tests);
    static void error_(const char *file, int line, const char *msg, int fail);

    template <typename T>
    static void checkBuffersEqual(const T *expected, const T *actual, size_t size)
    {
        std::vector<T> wrapped_expected(expected, expected + size);
        std::vector<T> wrapped_actual(actual, actual + size);
        if (wrapped_actual != wrapped_expected) {
            std::ostringstream ss;
            ss << "Error: Buffers not equal - expected=[";
            for (auto &&item : wrapped_expected) {
                ss << item << ",";
            }
            ss << "], actual=[";
            for (auto &&item : wrapped_actual) {
                ss << item << ",";
            }
            ss << "]";
            std::string msg = ss.str();
            CHECK_MSG( false, msg.c_str());
        }
    }

    template <typename T>
    static void checkBufferSorted(const T *buffer, size_t size, int sorted)
    {
        std::vector<T> sorted_buffer(buffer, buffer + size);
        switch (sorted) {
            case KNOWN_UNSORTED:
            case UNKNOWN_SORTEDNESS:
                return;
            case SORTED_INCR:
            case SORTED_INCR_NA_1ST:
                std::sort(sorted_buffer.begin(), sorted_buffer.end(), std::greater_equal<T>());
                break;
            case SORTED_DECR:
            case SORTED_DECR_NA_1ST:
                std::sort(sorted_buffer.begin(), sorted_buffer.end(), std::less_equal<T>());
                break;
            default:
                // TODO: throw exception?
                return;
        }

        std::vector<T> wrapped_buffer(buffer, buffer + size);
        CHECK_MSG( sorted_buffer == wrapped_buffer, "Buffer is not sorted");
    }

private:
    static const Test *curr_test;
    static std::vector< Test> failed_tests;

    static void addFailedTest();
    static void beforeRunAll();
    static void afterRunAll();
};


#endif // TESTS_HPP_
