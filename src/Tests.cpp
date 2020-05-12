#include "Tests.hpp"
#include <R.h>
#include <Rinternals.h>

const Test * Tests::curr_test = nullptr;
std::map< std::string, TestResult> Tests::test_results;

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

TestResult Test::execute() const
{
    Rprintf("===== Running %s ====== \n", m_name.c_str());
    TestResult result = m_func();
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
        TestResult result = test.execute();
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
            Rf_error("FW error: test dependency should be already run.\n");
        }
        TestResult dep_result = it->second;
        if (dep_result == TestResult::Failure) {
            return false;
        }
    }
    return true;
}

bool Tests::someTestFailed()
{
    auto it = std::find_if(test_results.cbegin(), test_results.cend(),
        [](std::pair<const std::string, TestResult> item){
            return item.second == TestResult::Failure;
        });
    return it != test_results.cend();
}

void Tests::addFailedTest()
{
    if (curr_test == nullptr) {
        error("curr_test should not be nullptr");
    }

    if (test_results.find(curr_test->getName()) != test_results.end()) {
        return;
    }

    test_results.insert(std::make_pair(curr_test->getName(), TestResult::Failure));
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
