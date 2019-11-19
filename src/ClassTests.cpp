#include "ClassTests.hpp"
#include <R.h>
#include <Rinternals.h>
#include "Tests.hpp"
#include <numeric>

const std::vector< Test> ClassTests::tests = {
    {"test_length", testLength},
    {"test_set_elt", testSetElt},
    {"test_dataptr", testDataptr},
    {"test_get_one_region", testGetOneRegion},
    {"test_get_more_regions", testGetMoreRegions},
    {"test_is_sorted", testIsSorted}
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
    std::srand(42);
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

    SKIP_IF_NOT( TYPEOF(instance) == INTSXP);

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
    SKIP_IF_NOT( TYPEOF(instance) == INTSXP);
    SKIP_IF_NOT( LENGTH(instance) > 5);

    SET_INTEGER_ELT(instance, 1, 1);
    SET_INTEGER_ELT(instance, 2, 2);

    int buf[2] = {0};
    int expected_buf[2] = {1, 2};
    R_xlen_t copied = INTEGER_GET_REGION(instance, 1, 2, buf);
    CHECK( copied > 0);
    Tests::checkBuffersEqual(expected_buf, buf, 2);
}

void ClassTests::testGetMoreRegions()
{
    SKIP_IF_NOT( TYPEOF(instance) == INTSXP);
    SKIP_IF_NOT( LENGTH(instance) > 10);

    // idx  =  0 1 2 3 4 5 6 7 8 9
    // vals = [x 1 1 1 x x x 2 2 2]
    const int region_1_from = 1;
    const int region_1_size = 3;
    const int region_1_value = 1;

    const int region_2_from = 8;
    const int region_2_size = 3;
    const int region_2_value = 2;

    for (int i = region_1_from; i < region_1_from + region_1_size; i++) {
        SET_INTEGER_ELT(instance, i, region_1_value);
    }

    for (int i = region_2_from; i < region_2_from + region_2_size; i++) {
        SET_INTEGER_ELT(instance, i, region_2_value);
    }

    int buf[3] = {0};
    int expected_buf[3] = {region_1_value, region_1_value, region_1_value};
    INTEGER_GET_REGION(instance, region_1_from, region_1_size, buf);
    Tests::checkBuffersEqual(expected_buf, buf, region_1_size);

    int expected_buf_2[3] = {region_2_value, region_2_value, region_2_value};
    INTEGER_GET_REGION(instance, region_2_from, region_2_size, buf);
    Tests::checkBuffersEqual(expected_buf_2, buf, region_2_size);
}

void ClassTests::testIsSorted()
{
    SKIP_IF_NOT( TYPEOF(instance) == INTSXP || TYPEOF(instance) == REALSXP);

    switch (TYPEOF(instance)) {
        case INTSXP: {
            int sorted = INTEGER_IS_SORTED(instance);
            Tests::checkBufferSorted(INTEGER(instance), LENGTH(instance), sorted);
            break;
        }
        case REALSXP: {
            int sorted = REAL_IS_SORTED(instance);
            Tests::checkBufferSorted(REAL(instance), LENGTH(instance), sorted);
            break;
        }
    }
}

void ClassTests::testSum()
{
    const int length = LENGTH(instance);
    SKIP_IF_NOT(length > 10);
    SKIP_IF_NOT(TYPEOF(instance) == INTSXP);

    for (R_xlen_t i = 0; i < length; i++) {
        const int value = std::rand() % length;
        SET_INTEGER_ELT(instance, i, value);
    }
    std::vector<int> vec(INTEGER(instance), INTEGER(instance) + length);
    int expected_sum = std::accumulate(vec.begin(), vec.end(), 0);

    SEXP actual_sum_sexp = ALTINTEGER_SUM(instance, TRUE);
    CHECK( LENGTH(actual_sum_sexp) == 1);
    CHECK( TYPEOF(actual_sum_sexp) == INTSXP);
    int actual_sum = INTEGER_ELT(actual_sum_sexp, 1);

    CHECK( actual_sum == expected_sum);
}
