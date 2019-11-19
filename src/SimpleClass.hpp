#ifndef SIMPLE_CLASS_HPP_
#define SIMPLE_CLASS_HPP_

#include "altrep_include.hpp"
#include <R_ext/Rdynload.h>
#include <R.h>
#include <Rinternals.h>

class SimpleClass {
public:
    static void init(DllInfo *info);
    static R_altrep_class_t getDescriptor();
    static SEXP createInstance();

private:
    static constexpr const char *name = "SimpleClass";
    static constexpr R_xlen_t vec_len = 30;
    static R_altrep_class_t descr;

    static R_xlen_t lengthMethod(SEXP instance);
    static void * dataptr(SEXP instance, Rboolean writeabble);
    static int elt(SEXP instance, R_xlen_t idx);
    static R_xlen_t getRegion(SEXP instance, R_xlen_t from_idx, R_xlen_t size, int *buffer);
    static SEXP getData(SEXP instance);
};

#endif // SIMPLE_CLASS_HPP_
