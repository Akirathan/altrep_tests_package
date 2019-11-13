#include "ClassTests.hpp"
#include <R.h>
#include <Rinternals.h>
#include "Tests.hpp"

const std::vector< Test> ClassTests::tests = {
    {"test_length", testLength},
    {"test_set_elt", testSetElt},
    {"test_dataptr", testDataptr},
    {"test_get_one_region", testGetOneRegion},
    {"test_get_more_regions", testGetMoreRegions}
};
SEXP ClassTests::instance;


SEXP ClassTests::runAll(SEXP instance)
{
    beforeRunAll(instance);
    bool succ = Tests::runAll(tests);
    afterRunAll();
    return ScalarLogical(succ);
}

void ClassTests::beforeRunAll(SEXP _instance)
{
    instance = _instance;
    R_PreserveObject(instance);
}

void ClassTests::afterRunAll()
{
    R_ReleaseObject(instance);
}

void ClassTests::testLength()
{
    CHECK_MSG (LENGTH(instance) > 0, "Length should be > 0");
}

void ClassTests::testSetElt()
{
    const int int_val = 42;
    const int real_val = 42.0;
    const void *data_ptr_old = DATAPTR(instance);
    const int idx = rand() % LENGTH(instance);

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
            warning("_test_set_elt for type %s not yet implemented.\n", type2char(TYPEOF(instance)));
    }

    CHECK_MSG( data_ptr_old == DATAPTR(instance), "DATAPTR should be pointer to same address, ie. no new instance should be allocated.");
}

void ClassTests::testDataptr()
{
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
}

void ClassTests::testGetOneRegion()
{
    if (TYPEOF(instance) != INTSXP) {
        Rprintf("\t%s for type %s not yet implemented.\n", __func__, type2char(TYPEOF(instance)));
        return;
    }
    if (LENGTH(instance) < 5) {
        Rprintf("Expected at least length 5 of instance.\n");
        return;
    }
    SET_INTEGER_ELT(instance, 1, 1);
    SET_INTEGER_ELT(instance, 2, 2);

    int buf[2] = {0};
    int expected_buf[2] = {1, 2};
    R_xlen_t copied = INTEGER_GET_REGION(instance, 1, 2, buf);
    CHECK( copied > 0);
    Tests::checkBuffers(expected_buf, buf, 2);
}

void ClassTests::testGetMoreRegions()
{
    // idx  =  0 1 2 3 4 5 6 7 8 9
    // vals = [x 1 1 1 x x x 2 2 2]
    const int region_1_from = 1;
    const int region_1_size = 3;
    const int region_1_value = 1;

    const int region_2_from = 8;
    const int region_2_size = 3;
    const int region_2_value = 2;

    if (TYPEOF(instance) != INTSXP) {
        Rprintf("Type %s not yet implemented, skipping %s\n", type2char(TYPEOF(instance)), __func__);
        return;
    }
    if (LENGTH(instance) < 10) {
        Rprintf("Expected at least length 10 of instance, skipping %s\n", __func__);
        return;
    }

    for (int i = region_1_from; i < region_1_from + region_1_size; i++) {
        SET_INTEGER_ELT(instance, i, region_1_value);
    }

    for (int i = region_2_from; i < region_2_from + region_2_size; i++) {
        SET_INTEGER_ELT(instance, i, region_2_value);
    }

    int buf[3] = {0};
    int expected_buf[3] = {region_1_value, region_1_value, region_1_value};
    INTEGER_GET_REGION(instance, region_1_from, region_1_size, buf);
    Tests::checkBuffers(expected_buf, buf, region_1_size);

    int expected_buf_2[3] = {region_2_value, region_2_value, region_2_value};
    INTEGER_GET_REGION(instance, region_2_from, region_2_size, buf);
    Tests::checkBuffers(expected_buf_2, buf, region_2_size);
}
