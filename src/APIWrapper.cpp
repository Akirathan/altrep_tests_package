#include "APIWrapper.hpp"


SEXP APIWrapper::R_new_altrep(R_altrep_class_t descr, SEXP data1, SEXP data2)
{
    return ::R_new_altrep(descr, data1, data2);
}

SEXP APIWrapper::R_altrep_data1(SEXP instance)
{
    return ::R_altrep_data1(instance);
}

SEXP APIWrapper::R_altrep_data2(SEXP instance)
{
    return ::R_altrep_data2(instance);
}
