#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>
#include "SimpleClass.hpp"
#include "ClassTests.hpp"
#include "FrameworkTests.hpp"


static SEXP simple_class_tests();
static SEXP simple_class_ctor();

static const R_CallMethodDef CallEntries[] = {
        {"framework_tests", (DL_FUNC) &FrameworkTests::run, 0},
        {"class_tests", (DL_FUNC) &ClassTests::runAll, 1},
        {"simple_class_tests", (DL_FUNC) &simple_class_tests, 0},
        {"simple_class_ctor", (DL_FUNC) &simple_class_ctor, 0},
        {NULL, NULL, 0}
};

extern "C" void R_init_altreptests(DllInfo *dll)
{
    SimpleClass::init(dll);
    FrameworkTests::init(dll);
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
}

static SEXP simple_class_tests()
{
    SEXP instance = PROTECT(SimpleClass::createInstance());
    bool succ = ClassTests::runAll(instance);
    UNPROTECT(1);
    return ScalarLogical(succ);
}

static SEXP simple_class_ctor()
{
    return SimpleClass::createInstance();
}
