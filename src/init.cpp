#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>
#include "SimpleClass.hpp"
#include "SimpleStringClass.hpp"
#include "ClassTests.hpp"
#include "FrameworkTests.hpp"

static SEXP is_altrep(SEXP x);

static const R_CallMethodDef CallEntries[] = {
        {"is_altrep", (DL_FUNC) &is_altrep, 1},
        {"framework_tests", (DL_FUNC) &FrameworkTests::run, 0},
        {"class_tests", (DL_FUNC) &ClassTests::runAll, 2},
        {"simple_class_new", (DL_FUNC) &SimpleClass::createInstance, 0},
        {"simple_string_class_new", (DL_FUNC) &SimpleStringClass::createInstance, 0},
        {NULL, NULL, 0}
};

extern "C" void R_init_altreptests(DllInfo *dll)
{
    SimpleClass::init(dll);
    FrameworkTests::init(dll);
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
}

SEXP is_altrep(SEXP x)
{
    if (ALTREP(x)) {
        return ScalarLogical(TRUE);
    }
    else {
        return ScalarLogical(FALSE);
    }
}
