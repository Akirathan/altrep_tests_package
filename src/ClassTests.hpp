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
    static bool testSetEltString();
    static bool testDataptr();
    static bool testStringIterate();
    static bool testDataptrRemainsSame();
    static bool testGetOneRegion();
    static bool testGetMoreRegions();
    static bool testIsSortedUnknown();
    static bool testIsSortedUnknownString();
    static bool testIsSortedIncreasing();
    static bool testSumInt();
    static bool testSumIntOverflow();
    static bool testMin();
    static bool testMax();
    static bool testCoerce();
    static bool testDuplicate();
};

#endif // CLASS_TESTS_HPP_
