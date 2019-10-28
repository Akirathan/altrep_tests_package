#include "class_tests.h"
#include "tests_common.h"

static R_altrep_class_t _class_descriptor;
static SEXP _instance_data1;
static SEXP _instance_data2;

static void _init_tests(R_altrep_class_t class_descriptor, SEXP instance_data1, SEXP instance_data2);
static void _deinit_tests();
static SEXP _new_instance();
static void _test_elt();
static void _test_dataptr();
static void _test_length_method();

static const test_t _tests[] = {
    {"test_elt", _test_elt},
    {"test_dataptr", _test_dataptr},
    {"test_length_method", _test_length_method},
    {NULL, NULL}
};

SEXP class_tests_run(R_altrep_class_t class_descriptor, SEXP instance_data1, SEXP instance_data2)
{
    _init_tests(class_descriptor, instance_data1, instance_data2);
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

static SEXP _new_instance()
{
    return R_new_altrep(_class_descriptor, _instance_data1, _instance_data2);
}

static void _test_elt()
{
    SEXP instance = PROTECT(_new_instance());
    const int int_val = 42;
    const int real_val = 42.0;
    const void *data_ptr_old = DATAPTR(instance);
    const int idx = rand() % LENGTH(instance);

    switch (TYPEOF(instance)) {
        case INTSXP:
            SET_INTEGER_ELT(instance, idx, int_val);
            CHECK( INTEGER_ELT(instance, idx) == int_val);
            break;
        case REALSXP:
            SET_REAL_ELT(instance, idx, real_val);
            CHECK( REAL_ELT(instance, idx) == real_val);
            break;
        default:
            warning("_test_elt for type %s not yet implemented.\n", type2char(TYPEOF(instance)));
    }

    CHECK_MSG( data_ptr_old == DATAPTR(instance), "DATAPTR should be pointer to same address, ie. no new instance should be allocated.");
    UNPROTECT(1);
}

static void _test_dataptr()
{
    SEXP instance = PROTECT(_new_instance());
    const void *dataptr_old = DATAPTR(instance);
    const int length = LENGTH(instance);

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
    SEXP instance = _new_instance();
    CHECK_MSG (LENGTH(instance) > 0, "Length should be > 0");
}
