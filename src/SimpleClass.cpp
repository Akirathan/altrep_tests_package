#include "SimpleClass.hpp"
#include "altrep_include.hpp"
#include <algorithm>

R_altrep_class_t SimpleClass::descr;

void SimpleClass::init(DllInfo *info)
{
    descr = R_make_altinteger_class(name, "altrepcpptests", info);

    // Override ALTREP methods.
    R_set_altrep_Length_method(descr, SimpleClass::lengthMethod);

    // Override ALTVEC methods.
    R_set_altvec_Dataptr_method(descr, SimpleClass::dataptr);

    // Override ALTINT methods.
    R_set_altinteger_Elt_method(descr, SimpleClass::elt);
    R_set_altinteger_Get_region_method(descr, SimpleClass::getRegion);
}

R_altrep_class_t SimpleClass::getDescriptor()
{
    return descr;
}

SEXP SimpleClass::createInstance()
{
    if (R_SEXP(descr) == NULL) {
        error("Must register class first");
    }

    SEXP data1 = allocVector(INTSXP, vec_len);
    std::fill(INTEGER(data1), INTEGER(data1) + vec_len, 42);
    return R_new_altrep(descr, data1, R_NilValue);
}

R_xlen_t SimpleClass::lengthMethod(SEXP instance)
{
    return vec_len;
}

void * SimpleClass::dataptr(SEXP instance, Rboolean writeabble)
{
    return DATAPTR(getData(instance));
}

int SimpleClass::elt(SEXP instance, R_xlen_t idx)
{
    if (0 <= idx && idx <= vec_len - 1) {
        return INTEGER_ELT(getData(instance), idx);
    }
    else {
        return R_NaInt;
    }
}

R_xlen_t SimpleClass::getRegion(SEXP instance, R_xlen_t from_idx, R_xlen_t size, int *buffer)
{
    SEXP data = getData(instance);
    if ((size >= vec_len) ||
        (!(0 <= from_idx && from_idx < vec_len)) ||
        buffer == nullptr ||
        TYPEOF(data) != INTSXP)
    {
        return R_NaInt;
    }

    int *data_ptr = INTEGER(data);
    memcpy(buffer, data_ptr + from_idx, size * sizeof(int));
    return size;
}

SEXP SimpleClass::getData(SEXP instance)
{
    return R_altrep_data1(instance);
}
