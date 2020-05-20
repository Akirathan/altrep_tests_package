#ifndef TESTS_HPP_
#define TESTS_HPP_

/**
 * TODO:
 *  - Different handling of package error (ie. my errors) from tests error like skipping
 *    duplicates, dependency not satisfied, etc.
 */

#include <R.h>
#include <Rinternals.h>
// TODO: This is a quickfix for clang - there is apperently a macro clash in <sstream> with length macro.
#undef length
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <algorithm>
#include <sstream>
#define length Rf_length


#define INIT_TEST \
    TestResult __result = TestResult::Success;

#define FINISH_TEST \
    return __result;

#define CHECK(cond) \
    if (!(cond)) { \
        Rprintf("CHECK failed at %s:%d\n", __FILE__, __LINE__); \
        __result = TestResult::Failure; \
    }

#define CHECK_MSG(cond, msg) \
    if (!(cond)) { \
        Rprintf("CHECK failed at %s:%d: %s\n", __FILE__, __LINE__, msg); \
        __result = TestResult::Failure; \
    }

#define ASSERT(cond) \
    if (!(cond)) { \
        Rf_error("%s:%d\n", __FILE__, __LINE__); \
    }

#define SKIP_IF_NOT(cond) \
    if (!(cond)) { \
        Rprintf("Precondition (at %s:%d) not satisfied, skipping test.\n", __FILE__, __LINE__); \
        return TestResult::Skip; \
    }


enum class TestResult {
    Success,
    Failure,
    Skip
};

class Test {
public:
    using func_type = std::function< TestResult(void)>;

    Test(const std::string &name, func_type func);
    Test(const std::string &name, func_type func, const std::vector< std::string> &deps);
    const std::string & getName() const;
    const std::vector< std::string> & getDependencies() const;
    TestResult execute() const;

private:
    std::string m_name;
    func_type m_func;
    std::vector< std::string> m_dependencies;
};

class Tests {
public:
    static bool runAll(const std::vector< Test> &tests);
    static void error_(const char *file, int line, const char *msg, int fail);

    template <typename T>
    static bool areBuffersEqual(const T *expected, const T *actual, size_t size)
    {
        std::vector<T> wrapped_expected(expected, expected + size);
        std::vector<T> wrapped_actual(actual, actual + size);
        return wrapped_actual == wrapped_expected;
    }

    template <typename T>
    static bool isBufferSorted(const T *buffer, size_t size, int sorted_mode)
    {
        std::vector<T> sorted_buffer(buffer, buffer + size);
        switch (sorted_mode) {
            case KNOWN_UNSORTED:
            case UNKNOWN_SORTEDNESS:
                return true;
            case SORTED_INCR:
            case SORTED_INCR_NA_1ST:
                std::sort(sorted_buffer.begin(), sorted_buffer.end(), std::less_equal<T>());
                break;
            case SORTED_DECR:
            case SORTED_DECR_NA_1ST:
                std::sort(sorted_buffer.begin(), sorted_buffer.end(), std::greater_equal<T>());
                break;
            default:
                // TODO: throw exception?
                return false;
        }

        std::vector<T> wrapped_buffer(buffer, buffer + size);
        return sorted_buffer == wrapped_buffer;
    }

private:
    static const Test *curr_test;
    static std::map< std::string, TestResult> test_results;

    static void runTest(const Test &test);
    static bool checkTestDependencies(const Test &test);
    static bool someTestFailed();
    static void addFailedTest();
    static void beforeRunAll();
    static void afterRunAll();
};


#endif // TESTS_HPP_
