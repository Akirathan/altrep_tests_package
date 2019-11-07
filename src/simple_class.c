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
static R_xlen_t simpleclass_get_region(SEXP x, R_xlen_t from_idx, R_xlen_t size, int *buffer);


R_altrep_class_t simple_class_register(DllInfo *dll)
{
    class_descriptor = R_make_altinteger_class(class_name, package_name, dll);

    // Override ALTREP methods.
    R_set_altrep_Length_method(class_descriptor, simpleclass_length);

    // Override ALTVEC methods.
    R_set_altvec_Dataptr_method(class_descriptor, simpleclass_dataptr);

    // Override ALTINT methods.
    R_set_altinteger_Elt_method(class_descriptor, simpleclass_elt);
    R_set_altinteger_Get_region_method(class_descriptor, simpleclass_get_region);

    if (R_SEXP(class_descriptor) == R_NilValue) {
        error("class_descriptor is NULL");
    }

    return class_descriptor;
}

SEXP simple_class_new_instance()
{
    if (R_SEXP(class_descriptor) == NULL) {
        error("Must register class first");
    }

    SEXP data1 = allocVector(INTSXP, VEC_LEN);
    return R_new_altrep(class_descriptor, data1, R_NilValue);
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

static R_xlen_t simpleclass_get_region(SEXP x, R_xlen_t from_idx, R_xlen_t size, int *buffer)
{
    SEXP data = SIMPLE_CLASS_DATA(x);
    if ((size >= VEC_LEN) ||
        (!(0 <= from_idx && from_idx < VEC_LEN)) ||
        buffer == NULL ||
        TYPEOF(data) != INTSXP)
    {
        return -1;
    }
    int *data_ptr = INTEGER(data);
    memcpy(buffer, data_ptr + from_idx, size * sizeof(int));
    LOG("simpleclass: GetRegion(from_idx=%d, size=%d, buffer=", from_idx, size);
    print_buffer(buffer, size);
    Rprintf(")\n");
    return size;
}
