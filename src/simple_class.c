#include "simple_class.h"
#include <string.h>


// =========== Static data ===========
#define VEC_LEN 5
static const char *package_name = "altreptests";
static const char *class_name = "simple_class";
static int data[VEC_LEN];
static R_altrep_class_t class_descriptor;

// =========== Static functions ===========
static void init_data();
// Methods to override
static SEXP simpleclass_ctor();
static R_xlen_t simpleclass_length(SEXP x);
static void * simpleclass_dataptr(SEXP x, Rboolean writeable);
static int simpleclass_elt(SEXP x, R_xlen_t i);


R_altrep_class_t register_simple_class(DllInfo *dll)
{
    init_data();

    class_descriptor = R_make_altinteger_class(class_name, package_name, dll);

    // Override ALTREP methods.
    R_set_altrep_Length_method(class_descriptor, simpleclass_length);

    // Override ALTVEC methods.
    R_set_altvec_Dataptr_method(class_descriptor, simpleclass_dataptr);

    // Override ALTINT methods.
    R_set_altinteger_Elt_method(class_descriptor, simpleclass_elt);

    if (R_SEXP(class_descriptor) == R_NilValue) {
        error("class_descriptor is NULL");
    }

    return class_descriptor;
}

static void init_data()
{
    memset(data, 0, VEC_LEN);
}


static SEXP simpleclass_ctor()
{
    SEXP new_instance = R_new_altrep(class_descriptor, R_NilValue, R_NilValue);
    return new_instance;
}

static R_xlen_t simpleclass_length(SEXP x)
{
    return VEC_LEN;
}

static void * simpleclass_dataptr(SEXP x, Rboolean writeable)
{
    return (void *)data;
}

static int simpleclass_elt(SEXP x, R_xlen_t i)
{
    if (1 <= i && i <= VEC_LEN) {
        return data[i];
    }
    else {
        return -1;
    }
}