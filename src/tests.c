#include "tests.h"
#include <R.h>
#include <Rinternals.h>
#include <R_ext/Altrep.h>
#include <stdio.h>
#include <stdlib.h>

static R_altrep_class_t class_descriptor;

static void _error(const char *file, int line, const char *msg, int fail);
static void test_altrep_inheritance();
static void test_struct_header();
static void test_instance_data();
static void test_set_integer_elt();
static void test_dataptr();
static void test_length_method();

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

typedef struct {
    SEXP data1;
    SEXP data2;
} instance_data_t;

static test_t tests[] = {
    {"test_altrep_inheritance", test_altrep_inheritance},
    {"test_struct_header", test_struct_header},
    {"test_instance_data", test_instance_data},
    {"test_set_integer_elt", test_set_integer_elt},
    {"test_dataptr", test_dataptr},
    {"test_length_method", test_length_method},
    {NULL, NULL}
};

void init_tests(R_altrep_class_t class_descr)
{
    // TODO: Implement duplicate?
    class_descriptor = class_descr;
    R_PreserveObject(R_SEXP(class_descriptor));
}

void deinit_tests()
{
    R_ReleaseObject(R_SEXP(class_descriptor));
}

SEXP run_all_tests()
{
    test_t *test = tests;
    while (test != NULL && test->name != NULL) {
        Rprintf("Running %s\n", test->name);
        test->func();
        Rprintf("Finished %s\n", test->name);
        test++;
    }
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

static void test_altrep_inheritance()
{
    SEXP instance = R_new_altrep(class_descriptor, R_NilValue, R_NilValue);
    CHECK( R_altrep_inherits(instance, class_descriptor));
}

static void test_struct_header()
{
    SEXP instance = R_new_altrep(class_descriptor, R_NilValue, R_NilValue);
    CHECK_MSG( ALTREP(instance), "Instance has not ALTREP bit set");
    // TODO: Type as parameter?
    CHECK_MSG( TYPEOF(instance) == INTSXP, "Instance should have INTSXP type");
}

static void test_instance_data()
{
    // data1 = [42], data2 = NA
	SEXP expected_instance_data1 = allocVector(INTSXP, 1);
	int *data_ptr = INTEGER(expected_instance_data1);
	*data_ptr = 42;
	SEXP expected_instance_data2 = R_NilValue;

    SEXP instance = R_new_altrep(class_descriptor, expected_instance_data1, expected_instance_data2);
	SEXP data1 = R_altrep_data1(instance);
	SEXP data2 = R_altrep_data2(instance);

	int default_flags = 16;
	CHECK_MSG( R_compute_identical(data1, expected_instance_data1, default_flags), "Instance data1 not identical");
	CHECK_MSG( R_compute_identical(data2, expected_instance_data2, default_flags), "Instance data2 not identical");
}

static void test_set_integer_elt()
{
    SEXP instance = R_new_altrep(class_descriptor, R_NilValue, R_NilValue);
    PROTECT(instance);
    void *data_ptr_old = DATAPTR(instance);

    SET_INTEGER_ELT(instance, 1, 42);
    CHECK( INTEGER_ELT(instance, 1) == 42);
    
    CHECK_MSG( data_ptr_old == DATAPTR(instance), "DATAPTR should be pointer to same address, ie. no new instance should be allocated.");
    UNPROTECT_PTR(instance);
}

static void test_dataptr()
{
    SEXP instance = R_new_altrep(class_descriptor, R_NilValue, R_NilValue);
    PROTECT(instance);
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
    UNPROTECT_PTR(instance);
}


static void test_length_method()
{
    SEXP instance = R_new_altrep(class_descriptor, R_NilValue, R_NilValue);
    CHECK_MSG (LENGTH(instance) > 0, "Length should be > 0");
}