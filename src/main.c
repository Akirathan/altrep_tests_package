#include "tests.h"
#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>
#include <R_ext/Altrep.h>
#include <string.h>

static const R_CallMethodDef CallEntries[] = {
        {"run_tests_for_descriptor", (DL_FUNC) &run_tests_for_descriptor, 3},
        {NULL, NULL, 0}
};

void R_init_altreptests(DllInfo *dll)
{
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
}
