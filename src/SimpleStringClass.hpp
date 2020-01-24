#ifndef _SIMPLE_STRING_CLASS_HPP_
#define _SIMPLE_STRING_CLASS_HPP_

#include "altrep_include.hpp"

class SimpleStringClass {
public:
    static SEXP createInstance();
    static void * Dataptr(SEXP instance, Rboolean writeabble);
    static const void * Dataptr_or_null(SEXP instance);
    static R_xlen_t Length(SEXP instance);
    static SEXP Elt(SEXP instance, R_xlen_t idx);
    static void Set_elt(SEXP instance, R_xlen_t idx, SEXP elem);
};

#endif