#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>
#include <R_ext/Altrep.h>
#include "framework_tests.h"
#include "class_tests.h"

static const R_CallMethodDef CallEntries[] = {
        {"class_tests", (DL_FUNC) &class_tests_run, 3},
        {"framework_tests", (DL_FUNC) &framework_tests_run, 0},
        {NULL, NULL, 0}
};

void R_init_altreptests(DllInfo *dll)
{
    framework_tests_init(dll);
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
}
