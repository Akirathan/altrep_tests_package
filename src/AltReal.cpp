#include "AltReal.hpp"

std::vector<double> AltReal::m_contents = {1.15, 2.13, 6, 9, 14.15};

SEXP AltReal::createInstance()
{
    R_altrep_class_t descr = R_make_altreal_class("AltReal", "altreptests", nullptr);
    R_set_altrep_Length_method(descr, Length);
    R_set_altvec_Dataptr_method(descr, Dataptr);
    return R_new_altrep(descr, R_NilValue, R_NilValue);
}

R_xlen_t AltReal::Length(SEXP instance)
{
    return m_contents.size();
}

void * AltReal::Dataptr(SEXP instance, Rboolean writeable)
{
    return m_contents.data();
}
