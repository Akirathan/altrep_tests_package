#include "class_tests.h"
#include "tests_common.h"

static SEXP _instance;

static void _init_tests(SEXP instance);
static void _deinit_tests();
static void _test_length_method();
static void _test_set_elt();
static void _test_dataptr();
static void _test_get_one_region();
static void _test_get_more_regions();

static const test_t _tests[] = {
    {"test_set_elt", _test_set_elt},
    {"test_dataptr", _test_dataptr},
    {"test_length_method", _test_length_method},
    {"test_get_one_region", _test_get_one_region},
    {"test_get_more_regions", _test_get_more_regions},
    {NULL, NULL}
};

SEXP class_tests_run(SEXP instance)
{
    ASSERT( instance != R_NilValue);
    if (!ALTREP(instance)) {
        return ScalarLogical(FALSE);
    }
    _init_tests(instance);
    Rboolean succ = run_all_tests(_tests);
    _deinit_tests();
    return ScalarLogical(succ);
}


static void _init_tests(SEXP instance)
{
    srand(42);
    // TODO: Duplicate?
    _instance = instance;
    R_PreserveObject(_instance);
}

static void _deinit_tests()
{
    R_ReleaseObject(_instance);
}

static void _test_length_method()
{
    CHECK_MSG (LENGTH(_instance) > 0, "Length should be > 0");
}

/**
 * This is actually also a DATAPTR tests, because SET_INTEGER_ELT expands to DATAPTR.
 */
static void _test_set_elt()
{
    const int int_val = 42;
    const int real_val = 42.0;
    const void *data_ptr_old = wrapper_dataptr(_instance);
    const int idx = rand() % wrapper_length(_instance);

    switch (TYPEOF(_instance)) {
        case INTSXP:
            SET_INTEGER_ELT(_instance, idx, int_val);
            CHECK( INTEGER_ELT(_instance, idx) == int_val);
            break;
#ifndef FASTR
        case REALSXP:
            SET_REAL_ELT(_instance, idx, real_val);
            CHECK( REAL_ELT(_instance, idx) == real_val);
            break;
#endif
        default:
            warning("_test_set_elt for type %s not yet implemented.\n", type2char(TYPEOF(_instance)));
    }

    CHECK_MSG( data_ptr_old == DATAPTR(_instance), "DATAPTR should be pointer to same address, ie. no new instance should be allocated.");
}

static void _test_dataptr()
{
    const void *dataptr_old = wrapper_dataptr(_instance);
    const int length = wrapper_length(_instance);

    if (TYPEOF(_instance) != INTSXP) {
        warning("\t%s for type %s not yet implemented.\n", __func__, type2char(TYPEOF(_instance)));
        return;
    }

    int *dataptr = INTEGER(_instance);
    for (int i = 0; i < length; i++) {
        dataptr[i] = i;
    }

    for (int i = 0; i < length; i++) {
        CHECK( INTEGER_ELT(_instance, i) == i);
    }

    CHECK_MSG( dataptr_old == DATAPTR(_instance), "DATAPTR should be pointer to same address, ie. no new instance should be allocated.");
}

static void _test_get_one_region()
{
    if (TYPEOF(_instance) != INTSXP) {
        Rprintf("\t%s for type %s not yet implemented.\n", __func__, type2char(TYPEOF(_instance)));
        return;
    }
    if (LENGTH(_instance) < 5) {
        Rprintf("Expected at least length 5 of instance.\n");
        return;
    }
    SET_INTEGER_ELT(_instance, 1, 1);
    SET_INTEGER_ELT(_instance, 2, 2);

    int buf[2] = {0};
    int expected_buf[2] = {1, 2};
    R_xlen_t copied = INTEGER_GET_REGION(_instance, 1, 2, buf);
    CHECK( copied > 0);
    check_buffers(expected_buf, buf, 2);
}

static void _test_get_more_regions()
{
    // idx  =  0 1 2 3 4 5 6 7 8 9
    // vals = [x 1 1 1 x x x 2 2 2]
    const int region_1_from = 1;
    const int region_1_size = 3;
    const int region_1_value = 1;

    const int region_2_from = 8;
    const int region_2_size = 3;
    const int region_2_value = 2;

    if (TYPEOF(_instance) != INTSXP) {
        Rprintf("Type %s not yet implemented, skipping %s\n", type2char(TYPEOF(_instance)), __func__);
        return;
    }
    if (LENGTH(_instance) < 10) {
        Rprintf("Expected at least length 10 of instance, skipping %s\n", __func__);
        return;
    }

    for (int i = region_1_from; i < region_1_from + region_1_size; i++) {
        SET_INTEGER_ELT(_instance, i, region_1_value);
    }

    for (int i = region_2_from; i < region_2_from + region_2_size; i++) {
        SET_INTEGER_ELT(_instance, i, region_2_value);
    }

    int buf[3] = {0};
    int expected_buf[3] = {region_1_value, region_1_value, region_1_value};
    INTEGER_GET_REGION(_instance, region_1_from, region_1_size, buf);
    check_buffers(expected_buf, buf, region_1_size);

    int expected_buf_2[3] = {region_2_value, region_2_value, region_2_value};
    INTEGER_GET_REGION(_instance, region_2_from, region_2_size, buf);
    check_buffers(expected_buf_2, buf, region_2_size);
}
