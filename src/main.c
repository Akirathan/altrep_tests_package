#include "main.h"
#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>
#include <R_ext/Altrep.h>
#include <string.h>

// =========== Static data ===========
static const char *package_name = "altreptests";
static const char *class_name = "class_name";
static int data[VEC_LEN];

// =========== Static functions ===========
// Init functions
static SEXP my_test();
static void init_data();
static void init_classes(DllInfo *dll);
// Methods to override
static SEXP myclass_ctor();
static R_xlen_t myclass_length(SEXP x);
static void * myclass_dataptr(SEXP x, Rboolean writeable);
static int myclass_elt(SEXP x, R_xlen_t i);


static const R_CallMethodDef CallEntries[] = {
        {"ctor", (DL_FUNC) &myclass_ctor, 0},
        {"run_tests", (DL_FUNC) &run_all_tests, 0},
        {NULL, NULL, 0}
};

void R_init_altreptests(DllInfo *dll)
{
    init_data();
    init_classes(dll);

    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
}

static void init_data()
{
    memset(data, 0, VEC_LEN);
}

static void init_classes(DllInfo *dll)
{
    myclass_descriptor = R_make_altinteger_class(class_name, package_name, dll);

    // Override ALTREP methods.
    R_set_altrep_Length_method(myclass_descriptor, myclass_length);

    // Override ALTVEC methods.
    R_set_altvec_Dataptr_method(myclass_descriptor, myclass_dataptr);

    // Override ALTINT methods.
    R_set_altinteger_Elt_method(myclass_descriptor, myclass_elt);
}


static SEXP myclass_ctor()
{
    SEXP new_instance = R_new_altrep(myclass_descriptor, R_NilValue, R_NilValue);
    return new_instance;
}

static R_xlen_t myclass_length(SEXP x)
{
    return VEC_LEN;
}

static void * myclass_dataptr(SEXP x, Rboolean writeable)
{
    return (void *)data;
}

static int myclass_elt(SEXP x, R_xlen_t i)
{
    if (1 <= i && i <= VEC_LEN) {
        return data[i];
    }
    else {
        return -1;
    }
}
