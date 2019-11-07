#ifndef SIMPLEMMAP_CLASS_TESTS_H
#define SIMPLEMMAP_CLASS_TESTS_H

#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>

void simplemmap_class_tests_init(DllInfo *dll);
void simplemmap_class_tests_deinit();
SEXP simplemmap_class_tests_run();

#endif //SIMPLEMMAP_CLASS_TESTS_H
