#include "Tests.hpp"
#include <R.h>
#include <Rinternals.h>

const Test * Tests::curr_test = nullptr;
std::vector< Test> Tests::failed_tests;

void Tests::error_(const char *file, int line, const char *msg, int fail)
{
    Rprintf("Error at %s:%d", file, line);
    if (msg)
        Rprintf(", msg=%s", msg);
    Rprintf("\n");
    if (fail) {
        error("Failure, exiting...");
    }
    addFailedTest();
}

bool Tests::runAll(const std::vector< Test> &tests)
{
    beforeRunAll();
    for (const Test &test : tests) {
        curr_test = &test;
        Rprintf("===== Running %s ====== \n", test.name.c_str());
        test.func();
        Rprintf("===== Finished %s ===== \n", test.name.c_str());
    }
    afterRunAll();
    return failed_tests.size() == 0;
}

void Tests::addFailedTest()
{
    if (curr_test == nullptr) {
        error("curr_test should not be nullptr");
    }

    for (const Test &test : failed_tests) {
        if (test.name == curr_test->name) {
            return;
        }
    }

    failed_tests.push_back(*curr_test);
}

void Tests::beforeRunAll()
{
    failed_tests.clear();
}

void Tests::afterRunAll()
{
    if (failed_tests.size() > 0) {
        Rprintf("Failures at:");
        for (const Test &test : failed_tests) {
            Rprintf("%s, ", test.name.c_str());
        }
        Rprintf("\n");
    }
}
