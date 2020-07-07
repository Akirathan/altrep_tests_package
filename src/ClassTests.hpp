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
    static void printInstanceInfo(SEXP instance);
    static void afterRunAll();
    static bool isWritable(SEXP instance);

    /**
     * Copy of data that is friendly for read-only altrep vectors ie. it does not invoke
     * Dataptr.
     */
    template <typename T>
    static std::vector<T> copyData(SEXP instance)
    {
        std::vector<T> vec(LENGTH(instance));
        for (int i = 0; i < LENGTH(instance); i++) {
            switch (TYPEOF(instance)) {
                case INTSXP:
                    vec.at(i) = INTEGER_ELT(instance, i);
                    break;
                case REALSXP:
                    vec.at(i) = REAL_ELT(instance, i);
                    break;
            }
        }
        return vec;
    }

    static TestResult testLength();
    static TestResult instanceIsStillAltrepAfterSet();
    static TestResult instanceIsStillAltrepAfterDataptr();
    static TestResult testSetElt();
    static TestResult testSetEltString();
    static TestResult testDataptr();
    static TestResult testStringIterate();
    static TestResult testDataptrRemainsSame();
    static TestResult getRegionWithoutPreset();
    static TestResult getTwoRegionsWithoutPreset();
    static TestResult getRegionWithPreset();
    static TestResult testGetMoreRegions();
    static TestResult testIsSortedUnknown();
    static TestResult testSortedness();
    static TestResult testIsSortedUnknownString();
    static TestResult testIsSortedIncreasing();
    static TestResult testSumIntWithPreset();
    static TestResult testSumIntWithoutPreset();
    static TestResult testMinWithPreset();
    static TestResult testMinWithoutPreset();
    static TestResult testMaxWithPreset();
    static TestResult testMaxWithoutPreset();
    static TestResult testCoerce();
    static TestResult testDuplicate();
    static TestResult noNAWithoutPreset();
    static TestResult noNAWithPreset();
};

#endif // CLASS_TESTS_HPP_
