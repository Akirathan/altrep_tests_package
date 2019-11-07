#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>
#include <R_ext/Altrep.h>
#include "tests_common.h"
#include "framework_tests.h"
#include "class_tests.h"
#include "simple_class.h"

static SEXP _simple_class_tests();

Rboolean DEBUG = TRUE;

static const R_CallMethodDef CallEntries[] = {
        {"class_tests", (DL_FUNC) &class_tests_run, 1},
        {"simple_class_tests", (DL_FUNC) &_simple_class_tests, 0},
        {"framework_tests", (DL_FUNC) &framework_tests_run, 0},
        {NULL, NULL, 0}
};

static R_altrep_class_t _simple_class_descr;

void R_init_altreptests(DllInfo *dll)
{
    framework_tests_init(dll);
    _simple_class_descr = simple_class_register(dll);
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
}

static SEXP _simple_class_tests()
{
    SEXP simple_class_instance = simple_class_new_instance();
    return class_tests_run(simple_class_instance);
}
