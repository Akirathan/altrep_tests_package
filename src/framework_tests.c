#include <R.h>
#include <Rinternals.h>
#include <R_ext/Altrep.h>
#include "framework_tests.h"
#include "tests_common.h"
#include "simple_class.h"

static R_altrep_class_t _class_descriptor;

static void _test_altrep_inheritance();
static void _test_struct_header();
static void _test_instance_data();
static void _test_modify_instance_data();
static void _test_set_instance_data();

static test_t _tests[] = {
    {"test_altrep_inheritance", _test_altrep_inheritance},
    {"test_struct_header", _test_struct_header},
    {"test_instance_data", _test_instance_data},
    {"test_modify_instance_data", _test_modify_instance_data},
    {"test_set_instance_data", _test_set_instance_data},
    {NULL, NULL}
};

void framework_tests_init(DllInfo *dll_info)
{
    _class_descriptor = simple_class_register(dll_info);
}

SEXP framework_tests_run()
{
    run_all_tests(_tests);
    return R_NilValue;
}

static SEXP _new_instance(R_altrep_class_t class_descriptor, SEXP data1, SEXP data2)
{
    SEXP instance = R_new_altrep(class_descriptor, data1, data2);
    if (DEBUG) {
        Rprintf("Framework tests: new instance=%d(0x%x)\n", instance, instance);
    }
    return instance;
}


static void _test_altrep_inheritance()
{
    SEXP instance = _new_instance(_class_descriptor, R_NilValue, R_NilValue);
    CHECK( R_altrep_inherits(instance, _class_descriptor));
}

static void _test_struct_header()
{
    SEXP instance = _new_instance(_class_descriptor, R_NilValue, R_NilValue);
    CHECK_MSG( ALTREP(instance), "Instance has not ALTREP bit set");
}

static void _test_instance_data()
{
    // data1 = [42], data2 = NA
	SEXP expected_instance_data1 = PROTECT(allocVector(INTSXP, 1));
	int *data_ptr = INTEGER(expected_instance_data1);
	*data_ptr = 42;
	SEXP expected_instance_data2 = R_NilValue;

    SEXP instance = _new_instance(_class_descriptor, expected_instance_data1, expected_instance_data2);

	int default_flags = 16;
	CHECK( R_compute_identical(R_altrep_data1(instance), expected_instance_data1, default_flags));
	CHECK( R_compute_identical(R_altrep_data2(instance), expected_instance_data2, default_flags));
    UNPROTECT(1);
}

static void _test_modify_instance_data()
{
    // data1 will be VecInt[], ale will be set to VecInt[42]
    SEXP int_vec = PROTECT(allocVector(INTSXP, 1));
    SEXP instance = PROTECT(_new_instance(_class_descriptor, int_vec, R_NilValue));

    SET_INTEGER_ELT(R_altrep_data1(instance), 0, 42);

    SEXP actual_data1 = R_altrep_data1(instance);
    CHECK( isInteger(actual_data1));
    CHECK( length(actual_data1) == 1);
    CHECK( INTEGER_ELT(actual_data1, 0) == 42);

    UNPROTECT(2);
}

static void _test_set_instance_data()
{
    SEXP instance = PROTECT(_new_instance(_class_descriptor, R_NilValue, R_NilValue));
    SEXP int_vec = PROTECT(ScalarInteger(42));
    SEXP lgl_vec = PROTECT(ScalarLogical(1));
    R_set_altrep_data1(instance, int_vec);
    R_set_altrep_data2(instance, lgl_vec);

    int default_flags = 16;
    CHECK( R_compute_identical(R_altrep_data1(instance), int_vec, default_flags));
    CHECK( R_compute_identical(R_altrep_data2(instance), lgl_vec, default_flags));

    UNPROTECT(3);
}
