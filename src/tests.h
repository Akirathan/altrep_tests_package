#include <R.h>
#include <Rinternals.h>
#include <R_ext/Altrep.h>

void init_tests(R_altrep_class_t class_descriptor);
void deinit_tests();
SEXP run_all_tests();
