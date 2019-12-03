#include "Tests.hpp"
#include <R.h>
#include <Rinternals.h>

const Test * Tests::curr_test = nullptr;
std::map< std::string, bool> Tests::test_results;

Test::Test(const std::string &name, func_type func)
    : m_name{name},
    m_func{func}
{
}

Test::Test(const std::string &name, func_type func, const std::vector< std::string> &deps)
    : m_name{name},
    m_func{func},
    m_dependencies(deps)
{
}

const std::string & Test::getName() const
{
    return m_name;
}

const std::vector< std::string> & Test::getDependencies() const
{
    return m_dependencies;
}

bool Test::execute() const
{
    Rprintf("===== Running %s ====== \n", m_name.c_str());
    bool result = m_func();
    Rprintf("===== Finished %s ====== \n", m_name.c_str());
    return result;
}

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
        runTest(test);
    }
    afterRunAll();
    return !someTestFailed();
}

void Tests::runTest(const Test &test)
{
    if (checkTestDependencies(test)) {
        bool result = test.execute();
        test_results.insert(std::make_pair(test.getName(), result));
    }
    else {
        Rprintf("%s: Some test dependency is not satisfied, skipping...\n", test.getName().c_str());
    }
}

bool Tests::checkTestDependencies(const Test &test)
{
    for (const std::string &dep_name : test.getDependencies()) {
        auto it = test_results.find(dep_name);
        if (it == test_results.end()) {
            error("FW error: test dependency should be already run.\n");
        }
        bool dep_result = it->second;
        if (!dep_result) {
            return false;
        }
    }
    return true;
}

bool Tests::someTestFailed()
{
    if (test_results.size() > 0) {
        for (auto &&item : test_results) {
            bool result = item.second;
            if (!result) {
                return true;
            }
        }
    }
    return false;
}

void Tests::addFailedTest()
{
    if (curr_test == nullptr) {
        error("curr_test should not be nullptr");
    }

    if (test_results.find(curr_test->getName()) != test_results.end()) {
        return;
    }

    test_results.insert(std::make_pair(curr_test->getName(), false));
}

void Tests::beforeRunAll()
{
    test_results.clear();
}

void Tests::afterRunAll()
{
    if (someTestFailed()) {
        Rprintf("Failures at:");
        for (auto &&item : test_results) {
            const std::string &test_name = item.first;
            bool result = item.second;
            if (!result) {
                Rprintf("%s, ", test_name.c_str());
            }
        }
        Rprintf("\n");
    }
}
