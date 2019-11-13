#ifndef CLASS_TESTS_HPP_
#define CLASS_TESTS_HPP_

#include "Tests.hpp"
#include "altrep_include.hpp"
#include <vector>

class ClassTests {
public:
    static SEXP runAll(SEXP instance);

private:
    static const std::vector< Test> tests;
    static SEXP instance;

    static void beforeRunAll(SEXP instance);
    static void afterRunAll();
    static void testLength();
    static void testSetElt();
    static void testDataptr();
    static void testGetOneRegion();
    static void testGetMoreRegions();
};

#endif // CLASS_TESTS_HPP_
