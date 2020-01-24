#include "SimpleStringClass.hpp"

SEXP SimpleStringClass::createInstance()
{
    auto descr = R_make_altstring_class("SimpleStringClass", "altreptests", nullptr);
    R_set_altvec_Dataptr_method(descr, &Dataptr);
    R_set_altvec_Dataptr_or_null_method(descr, &Dataptr_or_null);
    R_set_altrep_Length_method(descr, &Length);
    R_set_altstring_Elt_method(descr, &Elt);
    R_set_altstring_Set_elt_method(descr, &Set_elt);
    
    SEXP contents = Rf_allocVector(STRSXP, 2);
    SET_STRING_ELT(contents, 0, Rf_mkChar("ahoj"));
    SET_STRING_ELT(contents, 1, Rf_mkChar("Karle"));
    return R_new_altrep(descr, contents, R_NilValue);
}

void * SimpleStringClass::Dataptr(SEXP instance, Rboolean writeabble)
{
    return DATAPTR(R_altrep_data1(instance));
}

const void * SimpleStringClass::Dataptr_or_null(SEXP instance)
{
    return DATAPTR_OR_NULL(R_altrep_data1(instance));
}

R_xlen_t SimpleStringClass::Length(SEXP instance)
{
    return LENGTH(R_altrep_data1(instance));
}

SEXP SimpleStringClass::Elt(SEXP instance, R_xlen_t idx)
{
    return STRING_ELT(R_altrep_data1(instance), idx);
}

void SimpleStringClass::Set_elt(SEXP instance, R_xlen_t idx, SEXP elem)
{
    SET_STRING_ELT(R_altrep_data1(instance), idx, elem);
}
