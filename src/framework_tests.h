#ifndef FRAMEWORK_TESTS_H_
#define FRAMEWORK_TESTS_H_

#include <R_ext/Rdynload.h>

void framework_tests_init(DllInfo *info);
SEXP framework_tests_run();

#endif
