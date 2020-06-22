#include "ClassTests.hpp"
#include <R.h>
#include <Rinternals.h>
#include "Tests.hpp"
#include "utils.hpp"
#include <algorithm>
#include <numeric>
#include <limits>
#include <iostream>

#undef INIT_TEST
#define INIT_TEST \
    TestResult __result = TestResult::Success; \
    SEXP instance = Rf_eval(m_factory_method_call, m_rho); \
    PROTECT(instance);

#undef FINISH_TEST
#define FINISH_TEST \
    UNPROTECT(1); \
    return __result;


const std::vector< Test> ClassTests::tests = {
    {"testLength", testLength},
    {"instanceIsStillAltrepAfterSet", instanceIsStillAltrepAfterSet},
    {"instanceIsStillAltrepAfterDataptr", instanceIsStillAltrepAfterDataptr},
    {"testSetElt", testSetElt},
    {"testSetEltString", testSetEltString},
    {"testDataptr", testDataptr},
    {"testStringIterate", testStringIterate},
    {"testDataptrRemainsSame", testDataptrRemainsSame},
    {"testLengthRemainsSame", testLengthRemainsSame},
    {"getRegionWithoutPreset", getRegionWithoutPreset},
    {"getTwoRegionsWithoutPreset", getTwoRegionsWithoutPreset},
    {"getRegionWithPreset", getRegionWithPreset},
    {"testGetMoreRegions", testGetMoreRegions},
    {"testSortedness", testSortedness, {"testDataptr"}},
    {"testIsSortedUnknown", testIsSortedUnknown},
    {"testIsSortedUnknownString", testIsSortedUnknownString},
    {"testIsSortedIncreasing", testIsSortedIncreasing},
    {"testSumIntWithPreset", testSumIntWithPreset},
    {"testSumIntWithoutPreset", testSumIntWithoutPreset},
    {"testMinWithPreset", testMinWithPreset},
    {"testMinWithoutPreset", testMinWithoutPreset},
    {"testMaxWithPreset", testMaxWithPreset},
    {"testMaxWithoutPreset", testMaxWithoutPreset},
    {"testCoerce", testCoerce, {"testDataptr"}},
    {"testDuplicate", testDuplicate, {"testDataptr"}},
    {"noNAWithoutPreset", noNAWithoutPreset},
    {"noNAWithPreset", noNAWithPreset}
};
SEXP ClassTests::m_factory_method_call;
SEXP ClassTests::m_rho;


SEXP ClassTests::runAll(SEXP factory_method, SEXP rho)
{
    beforeRunAll(factory_method, rho);
    bool succ = Tests::runAll(tests);
    afterRunAll();
    return ScalarLogical(succ);
}

void ClassTests::beforeRunAll(SEXP factory_method, SEXP rho)
{
    if (TYPEOF(factory_method) != CLOSXP) {
        Rf_error("Expected type of factory_method is CLOSXP\n");
    }
    if (TYPEOF(rho) != ENVSXP) {
        Rf_error("Expected type of rho is environment\n");
    }

    std::srand(42);

    m_factory_method_call = Rf_lang1(factory_method);
    m_rho = rho;
    PROTECT(m_factory_method_call);
    PROTECT(m_rho);

    SEXP instance = PROTECT(Rf_eval(m_factory_method_call, m_rho));
    printInstanceInfo(instance);

    UNPROTECT(1);
}

void ClassTests::printInstanceInfo(SEXP instance)
{
    Rprintf("Instance info:{\n");
    if (ALTREP(instance)) {
        Rprintf("\tALTREP\n");
    }
    else {
        Rprintf("\tNot ALTREP\n");
    }
    Rprintf("\tTYPE = ");
    switch (TYPEOF(instance)) {
        case INTSXP:
            Rprintf("integer\n");
            break;
        case REALSXP:
            Rprintf("double\n");
            break;
        default:
            Rprintf("not yet implemented\n");
            break;
    }
    Rprintf("\tData = ");
    if (TYPEOF(instance) == INTSXP) {
        print_vector(copyData<int>(instance));
    }
    else if (TYPEOF(instance) == REALSXP) {
        print_vector(copyData<double>(instance));
    }
    Rprintf("\n");
    Rprintf("}\n");
}

void ClassTests::afterRunAll()
{
    UNPROTECT_PTR(m_factory_method_call);
    UNPROTECT_PTR(m_rho);
}

bool ClassTests::isWritable(SEXP instance)
{
    if (DATAPTR_OR_NULL(instance) == nullptr && MAYBE_SHARED(instance)) {
        return false;
    }
    else {
        return true;
    }
}

TestResult ClassTests::testLength()
{
    INIT_TEST;
    CHECK_MSG (LENGTH(instance) > 0, "Length should be > 0");
    FINISH_TEST;
}

/**
 * After invoking SET_*_ELT method on an altrep instance, it should still be altrep ie. the "worst"
 * thing that could happen is some memory allocation.
 */
TestResult ClassTests::instanceIsStillAltrepAfterSet()
{
    INIT_TEST;
    SKIP_IF_NOT( ALTREP(instance));
    SKIP_IF_NOT( TYPEOF(instance) == INTSXP || TYPEOF(instance) == REALSXP);
    if (TYPEOF(instance) == INTSXP) {
        SET_INTEGER_ELT(instance, 0, 42);
    }
    else if (TYPEOF(instance) == REALSXP) {
        SET_REAL_ELT(instance, 0, 42.5);
    }
    CHECK( ALTREP(instance));
    FINISH_TEST;
}

TestResult ClassTests::instanceIsStillAltrepAfterDataptr()
{
    INIT_TEST;
    SKIP_IF_NOT( ALTREP(instance));
    const void *dataptr = DATAPTR(instance);
    ASSERT( dataptr != nullptr);
    CHECK( ALTREP(instance));
    FINISH_TEST;
}

TestResult ClassTests::testSetElt()
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

TestResult ClassTests::testSetEltString()
{
    INIT_TEST;
    SKIP_IF_NOT( TYPEOF(instance) == STRSXP);
    SKIP_IF_NOT( LENGTH(instance) >= 2);

    // Set CHARSXP element in conventional way (via SET_STRING_ELT function).
    SET_STRING_ELT(instance, 0, Rf_mkChar("Ahoj0"));
    SEXP str_elt = STRING_ELT(instance, 0);
    ASSERT( TYPEOF(str_elt) == CHARSXP);
    CHECK( std::strcmp(Rf_translateChar(str_elt), "Ahoj0") == 0);

    // This may cause some problems with write barrier, but we do not care about
    // it here.
    SEXP *data = STRING_PTR(instance);
    data[1] = Rf_mkChar("Ahoj1");
    str_elt = STRING_ELT(instance, 1);
    ASSERT( TYPEOF(str_elt) == CHARSXP);
    CHECK( std::strcmp(Rf_translateChar(str_elt), "Ahoj1") == 0);

    FINISH_TEST;
}

/**
 * Very simple test of dataptr - we grab a pointer to native memory via Dataptr, then
 * modify some values inside native memory and check whether next invocation of
 * Dataptr method returns same pointer.
 */
TestResult ClassTests::testDataptr()
{
    INIT_TEST;
    SKIP_IF_NOT( TYPEOF(instance) == INTSXP);
    const void *dataptr_old = DATAPTR(instance);
    const int len = LENGTH(instance);


    int *dataptr = INTEGER(instance);
    for (int i = 0; i < len; i++) {
        dataptr[i] = i;
    }

    for (int i = 0; i < len; i++) {
        CHECK( INTEGER_ELT(instance, i) == i);
    }

    CHECK_MSG( dataptr_old == DATAPTR(instance), "DATAPTR should be pointer to same address, ie. no new instance should be allocated.");
    FINISH_TEST;
}

/**
 * Tests iteration over CHARSXP elements of one STRSXP.
 */
TestResult ClassTests::testStringIterate()
{
    INIT_TEST;
    SKIP_IF_NOT( TYPEOF(instance) == STRSXP);

    SEXP *data = STRING_PTR(instance);
    for (R_xlen_t i = 0; i < LENGTH(instance); i++) {
        CHECK( TYPEOF(data[i]) == CHARSXP);
    }

    for (R_xlen_t i = 0; i < LENGTH(instance); i++) {
        CHECK( TYPEOF(STRING_ELT(instance, i)) == CHARSXP);
    }

    FINISH_TEST;
}

/**
 * At the beggining we get a pointer to native memory via Dataptr method. Then we
 * do as much side effects on the altrep instance as possible - by invoking
 * various methods from that altrep instance, for example Sum, Min, etc.
 * 
 * The contract here is that DATAPTR(instance) should return same pointer as long
 * as instance is the same object.
 * 
 * This contract is trivially accomplished by for example integer vectors - for
 * those DATAPTR simply does some pointer arithmetics. However, for altrep
 * vectors this does not have to be true anymore. Theoretically, altrep vector
 * can return different pointer in every invocation of Dataptr, which breaks
 * the whole idea of immutability in R.
 */
TestResult ClassTests::testDataptrRemainsSame()
{
    INIT_TEST;
    SKIP_IF_NOT( TYPEOF(instance) != STRSXP);
    const void *dataptr_old = DATAPTR(instance);

    Rf_eval( Rf_lang2(Rf_install("sum"), instance), R_BaseEnv);
    CHECK(dataptr_old == DATAPTR(instance));

    Rf_eval( Rf_lang2(Rf_install("max"), instance), R_BaseEnv);
    CHECK(dataptr_old == DATAPTR(instance));

    Rf_eval( Rf_lang2(Rf_install("min"), instance), R_BaseEnv);
    CHECK(dataptr_old == DATAPTR(instance));

    Rf_coerceVector(instance, REALSXP);
    CHECK(dataptr_old == DATAPTR(instance));

    Rf_duplicate(instance);
    CHECK(dataptr_old == DATAPTR(instance));

    FINISH_TEST;
}

/**
 * As mentioned in the test above, all objects in R are immutable, which means that
 * a length of an object should not change.
 */
TestResult ClassTests::testLengthRemainsSame()
{
    INIT_TEST;
    SKIP_IF_NOT( TYPEOF(instance) != STRSXP);
    R_xlen_t old_length = LENGTH(instance);
    CHECK( old_length == LENGTH(instance));

    Rf_eval( Rf_lang2(Rf_install("sum"), instance), R_BaseEnv);
    CHECK(old_length == LENGTH(instance));

    Rf_eval( Rf_lang2(Rf_install("max"), instance), R_BaseEnv);
    CHECK(old_length == LENGTH(instance));

    Rf_eval( Rf_lang2(Rf_install("min"), instance), R_BaseEnv);
    CHECK(old_length == LENGTH(instance));

    Rf_coerceVector(instance, REALSXP);
    CHECK(old_length == LENGTH(instance));

    Rf_duplicate(instance);
    CHECK(old_length == LENGTH(instance));
    FINISH_TEST;
}

TestResult ClassTests::getRegionWithoutPreset()
{
    INIT_TEST;
    SKIP_IF_NOT( TYPEOF(instance) == INTSXP);

    std::vector<int> buf(LENGTH(instance));
    INTEGER_GET_REGION(instance, 0, LENGTH(instance), buf.data());

    std::vector<int> expected_data = copyData<int>(instance);

    CHECK( buf == expected_data);
    FINISH_TEST;
}

/**
 * Tests getting two overlaping regions.
 */
TestResult ClassTests::getTwoRegionsWithoutPreset()
{
    INIT_TEST;
    SKIP_IF_NOT( TYPEOF(instance) == INTSXP);
    SKIP_IF_NOT( LENGTH(instance) >= 10);

    const int region_1_from = 1;
    const int region_1_size = 5;
    const int region_2_from = 3;
    const int region_2_size = 6;

    const std::vector<int> expected_data = copyData<int>(instance);

    std::vector<int> buffer_1(region_1_size);
    R_xlen_t copied = INTEGER_GET_REGION(instance, region_1_from, region_1_size, buffer_1.data());
    CHECK( copied == region_1_size);
    for (int i = region_1_from, buffer_1_idx = 0; i < region_1_from + region_1_size; i++, buffer_1_idx++) {
        CHECK( expected_data.at(i) == buffer_1.at(buffer_1_idx));
    }

    std::vector<int> buffer_2(region_2_size);
    copied = INTEGER_GET_REGION(instance, region_2_from, region_2_size, buffer_2.data());
    CHECK( copied == region_2_size);
    for (int i = region_2_from, buffer_2_idx = 0; i < region_2_from + region_2_size; i++, buffer_2_idx++) {
        CHECK( expected_data.at(i) == buffer_2.at(buffer_2_idx));
    }

    FINISH_TEST;
}

TestResult ClassTests::getRegionWithPreset()
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

TestResult ClassTests::testGetMoreRegions()
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

TestResult ClassTests::testIsSortedUnknown()
{
    INIT_TEST;
    SKIP_IF_NOT( TYPEOF(instance) == INTSXP || TYPEOF(instance) == REALSXP);
    SKIP_IF_NOT( LENGTH(instance) > 3);
    SKIP_IF_NOT( isWritable(instance));

    // Set first few elements "randomly" so we get KNOWN_UNSORTED.
    // Note that as of R 3.6.1 SET_INTEGER_ELT does not dispatch into any altrep method, so
    // the instance does not know that someone has just modified some of its' elements.
    // So we add DATAPTR to be sure that the underlying instance knows that it is modified.
    // TODO: This should be fixed in future version of R.
    // Note that currently this test will fail for example for compact sequences.
    SET_INTEGER_ELT(instance, 0, 423);
    SET_INTEGER_ELT(instance, 1, 13);
    SET_INTEGER_ELT(instance, 2, 179);
    // TODO: This DATAPTR function call should be removed in future version of R. See comment
    // above. This is just a workaround.
    const void *dataptr = DATAPTR(instance);
    ASSERT( dataptr != nullptr);

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
    CHECK( sorted == KNOWN_UNSORTED || sorted == UNKNOWN_SORTEDNESS);
    FINISH_TEST;
}

TestResult ClassTests::testSortedness()
{
    INIT_TEST;
    SKIP_IF_NOT( TYPEOF(instance) == INTSXP);

    int sorted = INTEGER_IS_SORTED(instance);
    std::vector<int> vec = copyData<int>(instance);

    bool is_sorted = std::is_sorted(vec.cbegin(), vec.cend());
    if (sorted == UNKNOWN_SORTEDNESS) {
        // Do nothing
    }
    else if (sorted == KNOWN_UNSORTED) {
        CHECK_MSG( !is_sorted, "Expected unsorted vector");
    }
    else if (KNOWN_INCR(sorted)) {
        CHECK_MSG( is_sorted, "Expected sorted vector");
    }
    else if (KNOWN_DECR(sorted)) {
        CHECK_MSG(
            std::is_sorted(vec.cbegin(), vec.cend(), std::greater_equal<int>{}),
            "Expected sorted vector in decreasing order"
        );
    }
    FINISH_TEST;
}

TestResult ClassTests::testIsSortedUnknownString()
{
    INIT_TEST;
    SKIP_IF_NOT( TYPEOF(instance) == STRSXP);
    SKIP_IF_NOT( LENGTH(instance) > 3);

    // Set first few elements "randomly" so we get KNOWN_UNSORTED.
    SET_STRING_ELT(instance, 0, Rf_mkChar("xxx"));
    SET_STRING_ELT(instance, 1, Rf_mkChar("aaa"));
    SET_STRING_ELT(instance, 2, Rf_mkChar("zzz"));

    int sorted = STRING_IS_SORTED(instance);
    CHECK( sorted == KNOWN_UNSORTED || sorted == UNKNOWN_SORTEDNESS);
    FINISH_TEST;
}

TestResult ClassTests::testIsSortedIncreasing()
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

    CHECK( sorted_mode == SORTED_INCR || sorted_mode == UNKNOWN_SORTEDNESS);
    if (TYPEOF(instance) == INTSXP) {
        CHECK( Tests::isBufferSorted(INTEGER(instance), LENGTH(instance), sorted_mode));
    }
    else if (TYPEOF(instance) == REALSXP) {
        CHECK( Tests::isBufferSorted(REAL(instance), LENGTH(instance), sorted_mode));
    }
    FINISH_TEST;
}

TestResult ClassTests::testSumIntWithPreset()
{
    INIT_TEST;
    SKIP_IF_NOT( LENGTH(instance) > 10);
    SKIP_IF_NOT( TYPEOF(instance) == INTSXP);

    for (R_xlen_t i = 0; i < LENGTH(instance); i++) {
        SET_INTEGER_ELT(instance, i, 1);
    }
    int expected_sum = LENGTH(instance);

    // Note that there is nothing like Rf_sum that we could use from the native
    // side, so we are forced to call eval here. In fact, there is ALTINTEGER_SUM
    // function that is not hidden, but this will probably change.
    SEXP actual_sum_sexp = Rf_eval(Rf_lang2(Rf_install("sum"), instance), R_BaseEnv);
    ASSERT( actual_sum_sexp != nullptr);
    CHECK( LENGTH(actual_sum_sexp) == 1);
    CHECK( TYPEOF(actual_sum_sexp) == INTSXP);
    int actual_sum = INTEGER_ELT(actual_sum_sexp, 0);

    CHECK( actual_sum == expected_sum);
    FINISH_TEST;
}

TestResult ClassTests::testSumIntWithoutPreset()
{
    INIT_TEST;
    SKIP_IF_NOT( TYPEOF(instance) == INTSXP);
    std::vector<int> vec = copyData<int>(instance);
    int expected_sum = std::accumulate(vec.cbegin(), vec.cend(), 0);

    SEXP actual_sum = Rf_eval(Rf_lang2(Rf_install("sum"), instance), R_BaseEnv);
    ASSERT( actual_sum != nullptr);
    CHECK( LENGTH(actual_sum) == 1);
    CHECK( TYPEOF(actual_sum) == INTSXP);

    CHECK( expected_sum == INTEGER_ELT(actual_sum, 0));
    FINISH_TEST;
}

/**
 * Tests Min altrep method by presetting some values into the instance and then checking
 * the actual min element.
 */
TestResult ClassTests::testMinWithPreset()
{
    INIT_TEST;
    SKIP_IF_NOT( LENGTH(instance) > 10);
    SKIP_IF_NOT( TYPEOF(instance) == INTSXP);

    for (R_xlen_t i = 0; i < LENGTH(instance); i++) {
        const int value = std::rand() % LENGTH(instance);
        SET_INTEGER_ELT(instance, i, value);
    }
    std::vector<int> vec = copyData<int>(instance);
    int expected_min = *std::min_element(vec.cbegin(), vec.cend());

    SEXP actual_min = Rf_eval(Rf_lang2(Rf_install("min"), instance), R_BaseEnv);
    CHECK( LENGTH(actual_min) == 1);
    CHECK( TYPEOF(actual_min) == INTSXP);

    CHECK( expected_min == INTEGER_ELT(actual_min, 0));
    FINISH_TEST;
}

/**
 * Tests Min altrep method without modifying the instance.
 */
TestResult ClassTests::testMinWithoutPreset()
{
    INIT_TEST;
    SKIP_IF_NOT( TYPEOF(instance) == INTSXP);

    std::vector<int> vec = copyData<int>(instance);
    int expected_min = *std::min_element(vec.cbegin(), vec.cend());

    SEXP actual_min = Rf_eval(Rf_lang2(Rf_install("min"), instance), R_BaseEnv);
    CHECK( LENGTH(actual_min) == 1);
    CHECK( TYPEOF(actual_min) == INTSXP);

    CHECK( expected_min == INTEGER_ELT(actual_min, 0));
    FINISH_TEST;
}

TestResult ClassTests::testMaxWithPreset()
{
    INIT_TEST;
    const int length = LENGTH(instance);
    SKIP_IF_NOT( length > 10);
    SKIP_IF_NOT( TYPEOF(instance) == INTSXP);

    for (R_xlen_t i = 0; i < length; i++) {
        const int value = std::rand() % length;
        SET_INTEGER_ELT(instance, i, value);
    }
    std::vector<int> vec = copyData<int>(instance);
    int expected_max = *std::max_element(vec.begin(), vec.end());

    SEXP actual_max = Rf_eval(Rf_lang2(Rf_install("max"), instance), R_BaseEnv);
    CHECK( LENGTH(actual_max) == 1);
    CHECK( TYPEOF(actual_max) == INTSXP);

    CHECK( expected_max == INTEGER_ELT(actual_max, 0));
    FINISH_TEST;
}

TestResult ClassTests::testMaxWithoutPreset()
{
    INIT_TEST;
    SKIP_IF_NOT( TYPEOF(instance) == INTSXP);

    std::vector<int> vec = copyData<int>(instance);
    int expected_max = *std::max_element(vec.begin(), vec.end());

    SEXP actual_max = Rf_eval(Rf_lang2(Rf_install("max"), instance), R_BaseEnv);
    CHECK( LENGTH(actual_max) == 1);
    CHECK( TYPEOF(actual_max) == INTSXP);

    CHECK( expected_max == INTEGER_ELT(actual_max, 0));
    FINISH_TEST;
}

TestResult ClassTests::testCoerce()
{
    INIT_TEST;
    SKIP_IF_NOT( LENGTH(instance) > 10);
    SKIP_IF_NOT( TYPEOF(instance) == INTSXP);

    // We cannot invoke ALTREP_COERCE, because it is internal, and coerceVector
    // fallbacks to the default implementation if we do not provide a specific
    // one.
    SEXP coerced_vector = Rf_coerceVector(instance, REALSXP);
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
TestResult ClassTests::testDuplicate()
{
    INIT_TEST;
    SKIP_IF_NOT( TYPEOF(instance) == INTSXP);
    SEXP duplicated_instance = Rf_duplicate(instance);
    ASSERT( duplicated_instance != R_NilValue);
    CHECK( Tests::areBuffersEqual(INTEGER(instance),
                                  INTEGER(duplicated_instance),
                                  LENGTH(instance)));


    SEXP shallow_duplicated_instance = Rf_shallow_duplicate(instance);
    ASSERT( duplicated_instance != R_NilValue);
    CHECK( Tests::areBuffersEqual(INTEGER(instance),
                                  INTEGER(shallow_duplicated_instance),
                                  LENGTH(instance)));
    FINISH_TEST;
}

/**
 * The default implementation returns FALSE.
 */
TestResult ClassTests::noNAWithoutPreset()
{
    INIT_TEST;
    SKIP_IF_NOT( TYPEOF(instance) == INTSXP);

    std::vector<int> data = copyData<int>(instance);
    auto it = std::find(data.cbegin(), data.cend(), R_NaInt);
    Rboolean expected_no_na = it == data.cend() ? TRUE : FALSE;

    CHECK( INTEGER_NO_NA(instance) == expected_no_na ||
           INTEGER_NO_NA(instance) == FALSE); // Default implementation

    FINISH_TEST;
}

TestResult ClassTests::noNAWithPreset()
{
    INIT_TEST;
    SKIP_IF_NOT( TYPEOF(instance) == INTSXP);
    SKIP_IF_NOT( isWritable(instance));
    
    SET_INTEGER_ELT(instance, 0, R_NaInt);
    CHECK( FALSE == INTEGER_NO_NA(instance));

    for (int i = 0; i < LENGTH(instance); i++) {
        SET_INTEGER_ELT(instance, i, 0);
    }
    CHECK( TRUE == INTEGER_NO_NA(instance) ||
           FALSE == INTEGER_NO_NA(instance)); // Default implementation

    FINISH_TEST;
}
