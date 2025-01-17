#include "SimpleClass.hpp"
#include "altrep_include.hpp"
#include <algorithm>
#include <limits>
#include <vector>
#include <functional>

R_altrep_class_t SimpleClass::descr;

void SimpleClass::init(DllInfo *info)
{
    descr = R_make_altinteger_class(name, "altrepcpptests", info);

    // Override ALTREP methods.
    R_set_altrep_Length_method(descr, SimpleClass::lengthMethod);
    R_set_altrep_Duplicate_method(descr, SimpleClass::duplicate);

    // Override ALTVEC methods.
    R_set_altvec_Dataptr_method(descr, SimpleClass::dataptr);

    // Override ALTINT methods.
    R_set_altinteger_Elt_method(descr, SimpleClass::elt);
    R_set_altinteger_Get_region_method(descr, SimpleClass::getRegion);
    R_set_altinteger_Sum_method(descr, SimpleClass::sumMethod);
    R_set_altinteger_Min_method(descr, SimpleClass::minMethod);
    R_set_altinteger_Max_method(descr, SimpleClass::maxMethod);
    R_set_altinteger_Is_sorted_method(descr, SimpleClass::isSortedMethod);
    R_set_altinteger_No_NA_method(descr, SimpleClass::noNAMethod);
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

SEXP SimpleClass::duplicate(SEXP instance, Rboolean deep)
{
    SEXP data = getData(instance);
    SEXP ans = Rf_allocVector(INTSXP, LENGTH(data));
    for (int i = 0; i < LENGTH(data); i++) {
        INTEGER(ans)[i] = INTEGER_ELT(data, i);
    }
    return ans;
}

R_xlen_t SimpleClass::getRegion(SEXP instance, R_xlen_t from_idx, R_xlen_t size, int *buffer)
{
    SEXP data = getData(instance);
    if ((size > vec_len) ||
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

SEXP SimpleClass::sumMethod(SEXP instance, Rboolean narm)
{
    SEXP data = getData(instance);
    int sum = 0;
    for (int i = 0; i < LENGTH(data); i++) {
        sum += INTEGER_ELT(data, i);
    }
    return ScalarInteger(sum);
}

SEXP SimpleClass::minMethod(SEXP instance, Rboolean narm)
{
    SEXP data = getData(instance);
    int min = std::numeric_limits<int>::max();
    for (int i = 0; i < LENGTH(data); i++) {
        min = std::min(min, INTEGER_ELT(data, i));
    }
    return ScalarInteger(min);
}

SEXP SimpleClass::maxMethod(SEXP instance, Rboolean narm)
{
    SEXP data = getData(instance);
    int max = std::numeric_limits<int>::min();
    for (int i = 0; i < LENGTH(data); i++) {
        max = std::max(max, INTEGER_ELT(data, i));
    }
    return ScalarInteger(max);
}

int SimpleClass::isSortedMethod(SEXP instance)
{
    SEXP data = getData(instance);

    std::vector<int> vec(INTEGER(data), INTEGER(data) + LENGTH(data));
    std::vector<int> sorted_vec_incr(vec);
    std::vector<int> sorted_vec_decr(vec);
    std::sort(sorted_vec_incr.begin(), sorted_vec_incr.end(), std::less_equal<int>());
    std::sort(sorted_vec_decr.begin(), sorted_vec_decr.end(), std::greater_equal<int>());

    if (vec == sorted_vec_incr) {
        return SORTED_INCR;
    }
    else if (vec == sorted_vec_decr) {
        return SORTED_DECR;
    }
    else {
        return KNOWN_UNSORTED;
    }
}

int SimpleClass::noNAMethod(SEXP instance)
{
    return INTEGER_NO_NA(getData(instance));
}

SEXP SimpleClass::getData(SEXP instance)
{
    SEXP data = R_altrep_data1(instance);
    if (TYPEOF(data) != INTSXP) {
        Rf_error("SimpleClass: Expected INTSXP data");
    }
    return data;
}
