#ifndef API_WRAPPER_HPP_
#define API_WRAPPER_HPP_

#include "altrep_include.hpp"

class APIWrapper {
public:
    static SEXP R_new_altrep(R_altrep_class_t descr, SEXP data1, SEXP data2);
    static SEXP R_altrep_data1(SEXP instance);
    static SEXP R_altrep_data2(SEXP instance);
};

#endif //API_WRAPPER_HPP_
