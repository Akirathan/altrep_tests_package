#include "class_tests.h"
#include "tests_common.h"

static R_altrep_class_t _class_descriptor;
static SEXP _instance_data1;
static SEXP _instance_data2;

static void _init_tests(R_altrep_class_t class_descriptor, SEXP instance_data1, SEXP instance_data2);
static void _deinit_tests();
static void _print_addresses();
static void _test_elt();
static void _test_dataptr();
static void _test_length_method();
static void _test_get_one_region();
static void _test_get_more_regions();

static const test_t _tests[] = {
    {"test_elt", _test_elt},
    {"test_dataptr", _test_dataptr},
    {"test_length_method", _test_length_method},
    {"test_get_one_region", _test_get_one_region},
    {"test_get_more_regions", _test_get_more_regions},
    {NULL, NULL}
};

SEXP class_tests_run(R_altrep_class_t class_descriptor, SEXP instance_data1, SEXP instance_data2)
{
    _init_tests(class_descriptor, instance_data1, instance_data2);
    if (DEBUG) {
        _print_addresses();
    }
    run_all_tests(_tests);
    _deinit_tests();
    return R_NilValue;
}


static void _init_tests(R_altrep_class_t class_descriptor, SEXP instance_data1, SEXP instance_data2)
{
    srand(42);

    // TODO: Implement duplicate in FastR?
#ifdef FASTR
    _class_descriptor = class_descriptor;
#else
    R_SEXP(_class_descriptor) = duplicate(R_SEXP(class_descriptor));
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

static void _print_addresses()
{
    LOG("Class_tests addressess:\n");
    LOG("  class_descriptor = %d(0x%x)\n", R_SEXP(_class_descriptor), R_SEXP(_class_descriptor));
    LOG("  instance_data1 = %d(0x%x)\n", _instance_data1, _instance_data1);
    LOG("  instance_data2 = %d(0x%x)\n", _instance_data2, _instance_data2);
}

static void _test_elt()
{
    SEXP instance = PROTECT(wrapper_new_altrep(_class_descriptor, _instance_data1, _instance_data2));
    const int int_val = 42;
    const int real_val = 42.0;
    const void *data_ptr_old = wrapper_dataptr(instance);
    const int idx = rand() % wrapper_length(instance);

    switch (TYPEOF(instance)) {
        case INTSXP:
            SET_INTEGER_ELT(instance, idx, int_val);
            CHECK( INTEGER_ELT(instance, idx) == int_val);
            break;
#ifndef FASTR
        case REALSXP:
            SET_REAL_ELT(instance, idx, real_val);
            CHECK( REAL_ELT(instance, idx) == real_val);
            break;
#endif
        default:
            warning("_test_elt for type %s not yet implemented.\n", type2char(TYPEOF(instance)));
    }

    CHECK_MSG( data_ptr_old == DATAPTR(instance), "DATAPTR should be pointer to same address, ie. no new instance should be allocated.");
    UNPROTECT(1);
}

static void _test_dataptr()
{
    SEXP instance = PROTECT(wrapper_new_altrep(_class_descriptor, _instance_data1, _instance_data2));
    const void *dataptr_old = wrapper_dataptr(instance);
    const int length = wrapper_length(instance);

    if (TYPEOF(instance) != INTSXP) {
        warning("\t%s for type %s not yet implemented.\n", __func__, type2char(TYPEOF(instance)));
        return;
    }

    int *dataptr = INTEGER(instance);
    for (int i = 0; i < length; i++) {
        dataptr[i] = i;
    }

    for (int i = 0; i < length; i++) {
        CHECK( INTEGER_ELT(instance, i) == i);
    }

    CHECK_MSG( dataptr_old == DATAPTR(instance), "DATAPTR should be pointer to same address, ie. no new instance should be allocated.");
    UNPROTECT(1);
}

static void _test_length_method()
{
    SEXP instance = wrapper_new_altrep(_class_descriptor, _instance_data1, _instance_data2);
    CHECK_MSG (LENGTH(instance) > 0, "Length should be > 0");
}

static void _test_get_one_region()
{
    SEXP instance = PROTECT(wrapper_new_altrep(_class_descriptor, _instance_data1, _instance_data2));

    if (TYPEOF(instance) != INTSXP) {
        warning("\t%s for type %s not yet implemented.\n", __func__, type2char(TYPEOF(instance)));
        goto end;
    }
    if (LENGTH(instance) < 5) {
        warning("Expected at least length 5 of instance.\n");
        goto end;
    }
    SET_INTEGER_ELT(instance, 1, 1);
    SET_INTEGER_ELT(instance, 2, 2);

    int buf[2] = {0};
    INTEGER_GET_REGION(instance, 1, 2, buf);
    CHECK( buf[0] == 1);
    CHECK( buf[1] == 2);

end:
    UNPROTECT(1);
}

static void _test_get_more_regions()
{
    //  0 1 2 3 4 5 6 7 8 9
    // [x 1 1 1 x x x 2 2 2]
    const int region_1_from = 1;
    const int region_1_size = 3;
    const int region_1_value = 1;

    const int region_2_from = 8;
    const int region_2_size = 3;
    const int region_2_value = 2;

    SEXP instance = PROTECT(wrapper_new_altrep(_class_descriptor, _instance_data1, _instance_data2));

    if (TYPEOF(instance) != INTSXP) {
        warning("\t%s for type %s not yet implemented.\n", __func__, type2char(TYPEOF(instance)));
        goto end;
    }
    if (LENGTH(instance) < 10) {
        warning("Expected at least length 10 of instance.\n");
        goto end;
    }

    for (int i = region_1_from; i < region_1_from + region_1_size; i++) {
        SET_INTEGER_ELT(instance, i, region_1_value);
    }

    for (int i = region_2_from; i < region_2_from + region_2_size; i++) {
        SET_INTEGER_ELT(instance, i, region_2_value);
    }

    int buf[3] = {0};
    INTEGER_GET_REGION(instance, region_1_from, region_1_size, buf);
    for (int i = 0; i < region_1_size; i++) {
        CHECK( buf[i] == region_1_value);
    }
    INTEGER_GET_REGION(instance, region_2_from, region_2_size, buf);
    for (int i = 0; i < region_2_size; i++) {
        CHECK( buf[i] == region_2_value);
    }

end:
    UNPROTECT(1);
}
