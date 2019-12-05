#ifndef CLASS_TESTS_HPP_
#define CLASS_TESTS_HPP_

#include "Tests.hpp"
#include "altrep_include.hpp"
#include <vector>

// TODO: Convert to non-static class.
class ClassTests {
public:
    static SEXP runAll(SEXP instance);

private:
    static const std::vector< Test> tests;
    static SEXP instance;

    static void beforeRunAll(SEXP instance);
    static void afterRunAll();
    static bool testLength();
    static bool testSetElt();
    static bool testDataptr();
    static bool testGetOneRegion();
    static bool testGetMoreRegions();
    static bool testIsSortedUnknown();
    static bool testIsSortedIncreasing();
    static bool testSum();
    static bool testCoerce();
    static bool testDuplicate();
};

#endif // CLASS_TESTS_HPP_
