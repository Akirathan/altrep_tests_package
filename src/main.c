#include "tests.h"
#include "simple_class.h"
#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>
#include <R_ext/Altrep.h>
#include <string.h>

R_altrep_class_t g_class_descriptor;

static const R_CallMethodDef CallEntries[] = {
        {"run_tests", (DL_FUNC) &run_all_tests, 0},
        {NULL, NULL, 0}
};

void R_init_altreptests(DllInfo *dll)
{
    R_altrep_class_t simple_class_descr = register_simple_class(dll);
    // TODO: ??
    g_class_descriptor = simple_class_descr;
    init_tests(simple_class_descr);

    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
}

void R_unload_altreptests(DllInfo *dll)
{
    deinit_tests(dll);
}
