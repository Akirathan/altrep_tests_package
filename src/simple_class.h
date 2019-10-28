#ifndef SIMPLE_CLASS_H_
#define SIMPLE_CLASS_H_

#include <R.h>
#include <Rinternals.h>
#include <R_ext/Altrep.h>
#include <R_ext/Rdynload.h>
#include "tests_common.h"

R_altrep_class_t simple_class_register(DllInfo *info);
descr_with_data_t simple_class_get_descr_with_data();

#endif
