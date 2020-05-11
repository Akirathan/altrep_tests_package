#ifndef CLASS_TESTS_HPP_
#define CLASS_TESTS_HPP_

#include "Tests.hpp"
#include "altrep_include.hpp"
#include <vector>

// TODO: Convert to non-static class.
class ClassTests {
public:
    static SEXP runAll(SEXP factory_method, SEXP rho);

private:
    static const std::vector< Test> tests;
    static SEXP m_factory_method_call;
    static SEXP m_rho;

    static void beforeRunAll(SEXP instance, SEXP rho);
    static void afterRunAll();
    static bool isWritable(SEXP instance);
    template <typename T>
    static std::vector<T> copyData(SEXP instance);

    static bool testLength();
    static bool testSetElt();
    static bool testSetEltString();
    static bool testDataptr();
    static bool testStringIterate();
    static bool testDataptrRemainsSame();
    static bool testGetOneRegion();
    static bool testGetMoreRegions();
    static bool testIsSortedUnknown();
    static bool testSortedness();
    static bool testIsSortedUnknownString();
    static bool testIsSortedIncreasing();
    static bool testSumIntWithPreset();
    static bool testSumIntWithoutPreset();
    static bool testMinWithPreset();
    static bool testMinWithoutPreset();
    static bool testMaxWithPreset();
    static bool testMaxWithoutPreset();
    static bool testCoerce();
    static bool testDuplicate();
};

#endif // CLASS_TESTS_HPP_
