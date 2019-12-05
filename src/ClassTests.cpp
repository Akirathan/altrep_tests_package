#include "ClassTests.hpp"
#include <R.h>
#include <Rinternals.h>
#include "Tests.hpp"
#include <numeric>
#include <limits>
#include <iostream>

const std::vector< Test> ClassTests::tests = {
    {"test_length", testLength},
    {"test_set_elt", testSetElt},
    {"test_dataptr", testDataptr},
    {"test_get_one_region", testGetOneRegion},
    {"test_get_more_regions", testGetMoreRegions},
    {"test_is_sorted_unknown", testIsSortedUnknown},
    {"test_is_sorted_increasing", testIsSortedIncreasing},
    {"test_sum_int", testSumInt},
    // TODO: sumIntOverflow does not function as expected
    // {"test_sum_int_overflow", testSumIntOverflow},
    {"test_min", testMin},
    {"test_max", testMax},
    {"test_coerce", testCoerce, {"test_dataptr"}},
    {"test_duplicate", testDuplicate, {"test_dataptr"}}
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
    ASSERT( ALTREP(instance));
    R_PreserveObject(instance);
    std::srand(42);
}

void ClassTests::afterRunAll()
{
    R_ReleaseObject(instance);
}

bool ClassTests::testLength()
{
    INIT_TEST;
    CHECK_MSG (LENGTH(instance) > 0, "Length should be > 0");
    FINISH_TEST;
}

bool ClassTests::testSetElt()
{
    INIT_TEST;
    SKIP_IF_NOT( TYPEOF(instance) == INTSXP || TYPEOF(instance) == REALSXP);
    const void *data_ptr_old = DATAPTR(instance);
    const int idx = rand() % LENGTH(instance);

    switch (TYPEOF(instance)) {
        case INTSXP: {
            const int int_val = 42;
            SET_INTEGER_ELT(instance, idx, int_val);
            CHECK( INTEGER_ELT(instance, idx) == int_val);
            break;
        }
#ifndef FASTR
        case REALSXP: {
            const int real_val = 42.0;
            SET_REAL_ELT(instance, idx, real_val);
            CHECK( REAL_ELT(instance, idx) == real_val);
            break;
        }
#endif
    }

    CHECK_MSG( data_ptr_old == DATAPTR(instance), "DATAPTR should be pointer to same address, ie. no new instance should be allocated.");
    FINISH_TEST;
}

bool ClassTests::testDataptr()
{
    INIT_TEST;
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
    FINISH_TEST;
}

bool ClassTests::testGetOneRegion()
{
    INIT_TEST;
    SKIP_IF_NOT( TYPEOF(instance) == INTSXP);
    SKIP_IF_NOT( LENGTH(instance) > 5);

    SET_INTEGER_ELT(instance, 1, 1);
    SET_INTEGER_ELT(instance, 2, 2);

    int buf[2] = {0};
    int expected_buf[2] = {1, 2};
    R_xlen_t copied = INTEGER_GET_REGION(instance, 1, 2, buf);
    CHECK( copied == 2);
    Tests::areBuffersEqual(expected_buf, buf, 2);
    FINISH_TEST;
}

bool ClassTests::testGetMoreRegions()
{
    INIT_TEST;
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
    R_xlen_t copied = INTEGER_GET_REGION(instance, region_1_from, region_1_size, buf);
    CHECK( copied == region_1_size);
    CHECK( Tests::areBuffersEqual(expected_buf, buf, region_1_size));

    int expected_buf_2[3] = {region_2_value, region_2_value, region_2_value};
    copied = INTEGER_GET_REGION(instance, region_2_from, region_2_size, buf);
    CHECK( copied == region_2_size);
    CHECK( Tests::areBuffersEqual(expected_buf_2, buf, region_2_size));
    FINISH_TEST;
}

bool ClassTests::testIsSortedUnknown()
{
    INIT_TEST;
    SKIP_IF_NOT( TYPEOF(instance) == INTSXP || TYPEOF(instance) == REALSXP);
    SKIP_IF_NOT( LENGTH(instance) > 3);

    // Set first few elements "randomly" so we get KNOWN_UNSORTED.
    SET_INTEGER_ELT(instance, 0, 423);
    SET_INTEGER_ELT(instance, 1, 13);
    SET_INTEGER_ELT(instance, 2, 179);

    int sorted = UNKNOWN_SORTEDNESS;
    switch (TYPEOF(instance)) {
        case INTSXP: {
            sorted = INTEGER_IS_SORTED(instance);
            break;
        }
        case REALSXP: {
            sorted = REAL_IS_SORTED(instance);
            break;
        }
    }
    if (sorted == UNKNOWN_SORTEDNESS) {
        Rprintf("%s: Is_sorted has default implementation, skipping rest of test...\n", __func__);
        return true;
    }
    CHECK( sorted == KNOWN_UNSORTED);
    CHECK( Tests::isBufferSorted(INTEGER(instance), LENGTH(instance), sorted));
    FINISH_TEST;
}

bool ClassTests::testIsSortedIncreasing()
{
    INIT_TEST;
    SKIP_IF_NOT( TYPEOF(instance) == INTSXP || TYPEOF(instance) == REALSXP);

    for (int i = 0; i < LENGTH(instance); i++) {
        SET_INTEGER_ELT(instance, i, i);
    }

    int sorted_mode = UNKNOWN_SORTEDNESS;
    switch (TYPEOF(instance)) {
        case INTSXP:
            sorted_mode = INTEGER_IS_SORTED(instance);
            break;
        case REALSXP:
            sorted_mode = REAL_IS_SORTED(instance);
            break;
    }
    if (sorted_mode == UNKNOWN_SORTEDNESS) {
        Rprintf("%s: Is_sorted has default implementation, skipping rest of test...\n", __func__);
        return true;
    }
    CHECK( sorted_mode == SORTED_INCR);
    CHECK( Tests::isBufferSorted(INTEGER(instance), LENGTH(instance), sorted_mode));
    FINISH_TEST;
}

bool ClassTests::testSumInt()
{
    INIT_TEST;
    SKIP_IF_NOT(LENGTH(instance) > 10);
    SKIP_IF_NOT(TYPEOF(instance) == INTSXP);

    for (R_xlen_t i = 0; i < LENGTH(instance); i++) {
        SET_INTEGER_ELT(instance, i, 1);
    }
    int expected_sum = LENGTH(instance);

    // Note that there is no other "public" way how to invoke sum on altrep instance.
    // We could construct "sum" language element and evaluate it, but that would be
    // too complicated for these tests.
    // Therefore there is this "private" ALTINTEGER_SUM function call.
    // EDIT: Note that we do not use ALTINTEGER_SUM, although it is not hidden.
    SEXP sum_symbol = install("sum");
    SEXP sum_call = lang2(sum_symbol, instance);
    SEXP actual_sum_sexp = eval(sum_call, R_BaseEnv);
    ASSERT( actual_sum_sexp != nullptr);
    CHECK( LENGTH(actual_sum_sexp) == 1);
    CHECK( TYPEOF(actual_sum_sexp) == INTSXP);
    int actual_sum = INTEGER_ELT(actual_sum_sexp, 0);

    CHECK( actual_sum == expected_sum);
    FINISH_TEST;
}

bool ClassTests::testSumIntOverflow()
{
    INIT_TEST;
    const int length = LENGTH(instance);
    SKIP_IF_NOT(length > 10);
    SKIP_IF_NOT(TYPEOF(instance) == INTSXP);

    // Set the elements to some high constant so the Sum method overflows.
    SET_INTEGER_ELT(instance, 0, std::numeric_limits<int>::max());
    for (R_xlen_t i = 1; i < length; i++) {
        SET_INTEGER_ELT(instance, i, 1);
    }
    std::vector<int> vec(INTEGER(instance), INTEGER(instance) + length);

    double expected_sum = std::accumulate(vec.begin(), vec.end(), (double) 0.0);

    SEXP sum_symbol = install("sum");
    SEXP sum_call = lang2(sum_symbol, instance);
    SEXP actual_sum_sexp = eval(sum_call, R_BaseEnv);
    ASSERT( actual_sum_sexp != nullptr);
    CHECK( LENGTH(actual_sum_sexp) == 1);
    CHECK( TYPEOF(actual_sum_sexp) == REALSXP);
    double actual_sum = REAL_ELT(actual_sum_sexp, 0);

    CHECK( actual_sum == expected_sum);
    FINISH_TEST;
}

bool ClassTests::testMin()
{
    INIT_TEST;
    SKIP_IF_NOT(LENGTH(instance) > 10);
    SKIP_IF_NOT(TYPEOF(instance) == INTSXP);

    for (R_xlen_t i = 0; i < LENGTH(instance); i++) {
        const int value = std::rand() % LENGTH(instance);
        SET_INTEGER_ELT(instance, i, value);
    }
    std::vector<int> vec(INTEGER(instance), INTEGER(instance) + LENGTH(instance));
    int expected_min = *std::min_element(vec.begin(), vec.end());

    SEXP min_symbol = install("min");
    SEXP min_call = lang2(min_symbol, instance);
    SEXP actual_min_sexp = eval(min_call, R_BaseEnv);
    CHECK( LENGTH(actual_min_sexp) == 1);
    CHECK( TYPEOF(actual_min_sexp) == INTSXP);
    int actual_min = INTEGER_ELT(actual_min_sexp, 0);

    CHECK( expected_min == actual_min);
    FINISH_TEST;
}

bool ClassTests::testMax()
{
    INIT_TEST;
    const int length = LENGTH(instance);
    SKIP_IF_NOT(length > 10);
    SKIP_IF_NOT(TYPEOF(instance) == INTSXP);

    for (R_xlen_t i = 0; i < length; i++) {
        const int value = std::rand() % length;
        SET_INTEGER_ELT(instance, i, value);
    }
    std::vector<int> vec(INTEGER(instance), INTEGER(instance) + length);
    int expected_max = *std::max_element(vec.begin(), vec.end());

    SEXP max_symbol = install("max");
    SEXP max_call = lang2(max_symbol, instance);
    SEXP actual_max_sexp = eval(max_call, R_BaseEnv);
    CHECK( LENGTH(actual_max_sexp) == 1);
    CHECK( TYPEOF(actual_max_sexp) == INTSXP);
    int actual_max = INTEGER_ELT(actual_max_sexp, 0);

    CHECK( expected_max == actual_max);
    FINISH_TEST;
}

bool ClassTests::testCoerce()
{
    INIT_TEST;
    SKIP_IF_NOT( LENGTH(instance) > 10);
    SKIP_IF_NOT( TYPEOF(instance) == INTSXP);

    // We cannot invoke ALTREP_COERCE, because it is internal, and coerceVector
    // fallbacks to the default implementation if we do not provide a specific
    // one.
    SEXP coerced_vector = coerceVector(instance, REALSXP);
    ASSERT( coerced_vector != nullptr);
    ASSERT( coerced_vector != R_NilValue);
    CHECK( TYPEOF(coerced_vector) == REALSXP);
    CHECK( LENGTH(coerced_vector) == LENGTH(instance));

    for (int i = 0; i < LENGTH(instance); i++) {
        int instance_elem = INTEGER_ELT(instance, i);
        double coerced_elem = REAL_ELT(coerced_vector, i);
        int casted_elem = static_cast<int>(std::floor(coerced_elem));
        CHECK( instance_elem - 1 <= casted_elem && casted_elem <= instance_elem + 1);
    }

    FINISH_TEST;
}

/**
 * ALTREP_DUPLICATE is not called in GNU-R, only ALTREP_DUPLICATE_EX.
 * Called via duplicate or shallow_duplicate.
 * 
 * As in coerce there is no way to find out whether Duplicate method
 * is overriden in this ALTREP instance, therefore we have to call
 * duplicate public function.
 * 
 * See duplicate.c
 */
bool ClassTests::testDuplicate()
{
    INIT_TEST;
    SEXP duplicated_instance = duplicate(instance);
    ASSERT( duplicated_instance != R_NilValue);
    CHECK( Tests::areBuffersEqual(INTEGER(instance),
                                  INTEGER(duplicated_instance),
                                  LENGTH(instance)));


    SEXP shallow_duplicated_instance = shallow_duplicate(instance);
    ASSERT( duplicated_instance != R_NilValue);
    CHECK( Tests::areBuffersEqual(INTEGER(instance),
                                  INTEGER(shallow_duplicated_instance),
                                  LENGTH(instance)));
    FINISH_TEST;
}
