#ifndef FRAMEWORK_TESTS_HPP_
#define FRAMEWORK_TESTS_HPP_

#include <R_ext/Rdynload.h>
#include <vector>
#include "altrep_include.hpp"
#include "Tests.hpp"

class FrameworkTests {
public:
    static void init(DllInfo *dll);
    static SEXP run();

private:
    static const std::vector< Test> tests;
    static R_altrep_class_t simple_descr;

    static TestResult testAltrepInheritance();
    static TestResult testStructHeader();
    static TestResult testInstanceData();
    static TestResult testModifyInstanceData();
    static TestResult testSetInstanceData();
    static TestResult testDifferentDataptrValue();
    static TestResult testRedefineMethod();
};

#endif // FRAMEWORK_TESTS_HPP_