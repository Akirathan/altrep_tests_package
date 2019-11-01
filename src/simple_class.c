#include "simple_class.h"
#include <string.h>
#include "tests_common.h"


// =========== Static data ===========
#define VEC_LEN 15
static const char *package_name = "simpleclass";
static const char *class_name = "simple_class";
static R_altrep_class_t class_descriptor;

// =========== Static functions ===========
// Methods to override
static R_xlen_t simpleclass_length(SEXP x);
static void * simpleclass_dataptr(SEXP x, Rboolean writeable);
static int simpleclass_elt(SEXP x, R_xlen_t i);


R_altrep_class_t simple_class_register(DllInfo *dll)
{
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

descr_with_data_t simple_class_get_descr_with_data()
{
    if (R_SEXP(class_descriptor) == NULL) {
        error("Must register class first");
    }

    SEXP sample_data1 = allocVector(INTSXP, VEC_LEN);
    SEXP sample_data2 = R_NilValue;
    descr_with_data_t descr_with_data = { class_descriptor, sample_data1, sample_data2};
    return descr_with_data;
}

#define SIMPLE_CLASS_DATA(x)  R_altrep_data1(x)

static R_xlen_t simpleclass_length(SEXP x)
{
    LOG("simpleclass: Length(x=%d(0x%x))\n", x, x);
    return VEC_LEN;
}

static void * simpleclass_dataptr(SEXP x, Rboolean writeable)
{
    LOG("simpleclass: Dataptr(x=%d(0x%x), writeable=%d)\n", x, x, writeable);
    return DATAPTR(SIMPLE_CLASS_DATA(x));
}

static int simpleclass_elt(SEXP x, R_xlen_t i)
{
    LOG("simpleclass: Elt(x=%d(0x%x), i=%d)\n", x, x, i);
    if (0 <= i && i <= VEC_LEN - 1) {
        return INTEGER_ELT(SIMPLE_CLASS_DATA(x), i);
    }
    else {
        return -1;
    }
}