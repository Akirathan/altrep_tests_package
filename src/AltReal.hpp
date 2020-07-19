#ifndef ALT_REAL_HPP_
#define ALT_REAL_HPP_

#include <R.h>
#include <Rinternals.h>
#include "altrep_include.hpp"
#include <vector>

class AltReal {
public:
    static SEXP createInstance();
private:
    static std::vector<double> m_contents;

    static R_xlen_t Length(SEXP instance);
    static void * Dataptr(SEXP instance, Rboolean writeable);
};

#endif