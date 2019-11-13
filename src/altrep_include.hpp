#ifndef ALTREP_INCLUDE_HPP_
#define ALTREP_INCLUDE_HPP_

#include <R.h>
#include <Rinternals.h>

#define class klazz
extern "C" {
#include <R_ext/Altrep.h>
}
#undef class

#endif // ALTREP_INCLUDE_HPP_
