#include "tests.h"
#include <R.h>
#include <Rinternals.h>
#include <R_ext/Altrep.h>
#include <stdio.h>
#include <stdlib.h>


#define CHECK(cond) \
    if (!(cond)) \
        _error(__FILE__, __LINE__, NULL, 0)

#define CHECK_MSG(cond, msg) \
    if (!(cond)) \
        _error(__FILE__, __LINE__, msg, 0)

#define ASSERT(cond) \
    if (!(cond)) \
        _error(__FILE__, __LINE__, NULL, 1)

typedef struct {
    char *name;
    void (*func)(void);
} test_t;

static R_altrep_class_t _class_descriptor;
static SEXP _instance_data1;
static SEXP _instance_data2;

static void _error(const char *file, int line, const char *msg, int fail);
static void _init_tests(R_altrep_class_t class_descr, SEXP data1, SEXP data2);
static void _deinit_tests();
static SEXP _new_instance();
static void _test_altrep_inheritance();
static void _test_struct_header();
static void _test_instance_data();
static void _test_modify_instance_data();
static void _test_set_instance_data();
static void _test_set_integer_elt();
static void _test_dataptr();
static void _test_length_method();


static test_t tests[] = {
    {"test_altrep_inheritance", _test_altrep_inheritance},
    {"test_struct_header", _test_struct_header},
    {"test_instance_data", _test_instance_data},
    {"test_modify_instance_data", _test_modify_instance_data},
    {"test_set_instance_data", _test_set_instance_data},
    {"test_set_integer_elt", _test_set_integer_elt},
    {"test_dataptr", _test_dataptr},
    {"test_length_method", _test_length_method},
    {NULL, NULL}
};

SEXP run_tests_for_descriptor(R_altrep_class_t class_descr, SEXP data1, SEXP data2)
{
    _init_tests(class_descr, data1, data2);

    test_t *test = tests;
    while (test != NULL && test->name != NULL) {
        Rprintf("Running %s\n", test->name);
        test->func();
        Rprintf("Finished %s\n", test->name);
        test++;
    }

    _deinit_tests();
    return R_NilValue;
}

static void _error(const char *file, int line, const char *msg, int fail)
{
    Rprintf("Error at %s:%d", file, line);
    if (msg)
        Rprintf(", msg=%s", msg);
    Rprintf("\n");
    if (fail) {
        error("Failure, exiting...");
    }
}

static void _init_tests(R_altrep_class_t class_descr, SEXP instance_data1, SEXP instance_data2)
{
    // TODO: Implement duplicate?
#ifdef FASTR
    _class_descriptor = class_descr;
#else
    R_SEXP(_class_descriptor) = duplicate(R_SEXP(class_descr));
#endif
    R_PreserveObject(R_SEXP(_class_descriptor));

    // Copy instance data.
    _instance_data1 = duplicate(instance_data1);
    _instance_data2 = duplicate(instance_data2);
    R_PreserveObject(_instance_data1);
    R_PreserveObject(_instance_data2);
}

static void _deinit_tests()
{
    R_ReleaseObject(R_SEXP(_class_descriptor));
    R_ReleaseObject(_instance_data1);
    R_ReleaseObject(_instance_data2);
}

static SEXP _new_instance()
{
    return R_new_altrep(_class_descriptor, _instance_data1, _instance_data2);
}

static void _test_altrep_inheritance()
{
    SEXP instance = _new_instance();
    CHECK( R_altrep_inherits(instance, _class_descriptor));
}

static void _test_struct_header()
{
    SEXP instance = _new_instance();
    CHECK_MSG( ALTREP(instance), "Instance has not ALTREP bit set");
}

static void _test_instance_data()
{
    // data1 = [42], data2 = NA
	SEXP expected_instance_data1 = allocVector(INTSXP, 1);
	int *data_ptr = INTEGER(expected_instance_data1);
	*data_ptr = 42;
	SEXP expected_instance_data2 = R_NilValue;

    SEXP instance = R_new_altrep(_class_descriptor, expected_instance_data1, expected_instance_data2);
	SEXP data1 = R_altrep_data1(instance);
	SEXP data2 = R_altrep_data2(instance);

	int default_flags = 16;
	CHECK_MSG( R_compute_identical(data1, expected_instance_data1, default_flags), "Instance data1 not identical");
	CHECK_MSG( R_compute_identical(data2, expected_instance_data2, default_flags), "Instance data2 not identical");
}

static void _test_modify_instance_data()
{
    // data1 will be VecInt[], ale will be set to VecInt[42]
    SEXP int_vec = PROTECT(allocVector(INTSXP, 1));
    SEXP instance = PROTECT(R_new_altrep(_class_descriptor, int_vec, R_NilValue));

    // TODO: More generic type.
    SET_INTEGER_ELT(R_altrep_data1(instance), 0, 42);

    SEXP actual_data1 = R_altrep_data1(instance);
    CHECK( isInteger(actual_data1));
    CHECK( length(actual_data1) == 1);
    CHECK( INTEGER_ELT(actual_data1, 0) == 42);

    UNPROTECT(2);
}

static void _test_set_instance_data()
{
    SEXP instance = PROTECT(_new_instance());
    SEXP int_vec = PROTECT(ScalarInteger(42));
    SEXP lgl_vec = PROTECT(ScalarLogical(1));
    R_set_altrep_data1(instance, int_vec);
    R_set_altrep_data2(instance, lgl_vec);

    int default_flags = 16;
    CHECK( R_compute_identical(R_altrep_data1(instance), int_vec, default_flags));
    CHECK( R_compute_identical(R_altrep_data2(instance), lgl_vec, default_flags));

    UNPROTECT(3);
}

static void _test_set_integer_elt()
{
    SEXP instance = PROTECT(_new_instance());
    void *data_ptr_old = DATAPTR(instance);

    // TODO: index 0?
    SET_INTEGER_ELT(instance, 1, 42);
    CHECK( INTEGER_ELT(instance, 1) == 42);
    
    CHECK_MSG( data_ptr_old == DATAPTR(instance), "DATAPTR should be pointer to same address, ie. no new instance should be allocated.");
    UNPROTECT(1);
}

static void _test_dataptr()
{
    SEXP instance = PROTECT(_new_instance());
    void *dataptr_old = DATAPTR(instance);

    int *dataptr = INTEGER(instance);
    const int length = LENGTH(instance);
    for (int i = 0; i < length; i++) {
        dataptr[i] = i;
    }

    for (int i = 0; i < length; i++) {
        char *error_msg = (char *) malloc(128);
        sprintf(error_msg, "Element at index %d should be %d, but is %d.", i, i, INTEGER_ELT(instance, i));
        CHECK_MSG( INTEGER_ELT(instance, i) == i, error_msg);
        free(error_msg);
    }

    CHECK_MSG( dataptr_old == DATAPTR(instance), "DATAPTR should be pointer to same address, ie. no new instance should be allocated.");
    UNPROTECT(1);
}


static void _test_length_method()
{
    SEXP instance = _new_instance();
    CHECK_MSG (LENGTH(instance) > 0, "Length should be > 0");
}