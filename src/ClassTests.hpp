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
                    vec.push_back(INTEGER_ELT(instance, i));
                    break;
                case REALSXP:
                    vec.push_back(REAL_ELT(instance, i));
                    break;
            }
        }
        return vec;
    }

    static TestResult testLength();
    static TestResult testSetElt();
    static TestResult testSetEltString();
    static TestResult testDataptr();
    static TestResult testStringIterate();
    static TestResult testDataptrRemainsSame();
    static TestResult testGetOneRegion();
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
};

#endif // CLASS_TESTS_HPP_
