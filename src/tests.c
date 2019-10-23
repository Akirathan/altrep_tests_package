#include "main.h"
#include <R.h>
#include <Rinternals.h>
#include <R_ext/Altrep.h>

static R_altrep_class_t class_descriptor;

static void test_altrep_inheritance();
static void test_struct_header();
static void test_instance_data();
static void test_set_integer_elt();
static void test_dataptr();
static void test_length_method();

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

SEXP run_all_tests()
{
    size_t test_idx = 0;
    test_t *test = tests;
    while (test != NULL) {
        Rprintf("Running %s", test->name);
        test->func();
        Rprintf("Finished %s", test->name);
        test++;
    }
    return R_NilValue;
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
    CHECK (INTEGER_ELT(instance, 0) == 0);
    CHECK (INTEGER_ELT(instance, 1) == 42);
    CHECK (INTEGER_ELT(instance, 2) == 0);
    CHECK (INTEGER_ELT(instance, 3) == 0);
    CHECK (INTEGER_ELT(instance, 4) == 0);
    
    CHECK_MSG( data_ptr_old == DATAPTR(instance), "DATAPTR should be pointer to same address, ie. no new instance should be allocated.");
    UNPROTECT_PTR(instance);
}

static void test_dataptr()
{
    SEXP instance = R_new_altrep(class_descriptor, R_NilValue, R_NilValue);
    PROTECT(instance);
    void *dataptr_old = DATAPTR(instance);

    int *dataptr = INTEGER(instance);
    for (int i = 0; i < VEC_LEN; i++) {
        dataptr[i] = i;
    }

    for (int i = 0; i < VEC_LEN; i++) {
        CHECK( INTEGER_ELT(instance, i) == i);
    }

    CHECK_MSG( dataptr_old == DATAPTR(instance), "DATAPTR should be pointer to same address, ie. no new instance should be allocated.");
    UNPROTECT_PTR(instance);
}


static void test_length_method()
{
    SEXP instance = R_new_altrep(class_descriptor, R_NilValue, R_NilValue);
    int expected_length = VEC_LEN;
    CHECK_MSG (LENGTH(instance) == expected_length, "Length of instance should be 5 (VEC_LEN)");
}