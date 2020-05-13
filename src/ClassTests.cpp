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
    {"test_length", testLength},
    {"test_set_elt", testSetElt},
    {"test_set_elt_string", testSetEltString},
    {"test_dataptr", testDataptr},
    {"test_string_iterate", testStringIterate},
    {"test_dataptr_remains_same", testDataptrRemainsSame},
    {"test_get_one_region", testGetOneRegion},
    {"test_get_more_regions", testGetMoreRegions},
    {"test_sortedness", testSortedness, {"test_dataptr"}},
    {"test_is_sorted_unknown", testIsSortedUnknown},
    {"test_is_sorted_unknown_string", testIsSortedUnknownString},
    {"test_is_sorted_increasing", testIsSortedIncreasing},
    {"test_sum_int_with_preset", testSumIntWithPreset},
    {"test_sum_int_without_preset", testSumIntWithoutPreset},
    {"test_min_with_preset", testMinWithPreset},
    {"test_min_without_preset", testMinWithoutPreset},
    {"test_max_with_preset", testMaxWithPreset},
    {"test_max_without_preset", testMaxWithoutPreset},
    {"test_coerce", testCoerce, {"test_dataptr"}},
    {"test_duplicate", testDuplicate, {"test_dataptr"}}
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
    if (TYPEOF(factory_method) != LANGSXP) {
        Rf_error("Expected type of factory_method LANGSXP\n");
    }
    if (TYPEOF(rho) != ENVSXP) {
        Rf_error("Expected type of rho is environment\n");
    }

    std::srand(42);

    m_factory_method_call = factory_method;
    m_rho = rho;
    PROTECT(m_factory_method_call);
    PROTECT(m_rho);

    SEXP instance = PROTECT(Rf_eval(m_factory_method_call, m_rho));
    ASSERT( ALTREP(instance));
    printInstanceInfo(instance);

    UNPROTECT(1);
}

void ClassTests::printInstanceInfo(SEXP instance)
{
    if (TYPEOF(instance) == INTSXP && LENGTH(instance) < 100) {
        std::vector<int> data = copyData<int>(instance);
        Rprintf("Data of instance = ");
        print_vector(data);
        Rprintf("\n");
    }
}

void ClassTests::afterRunAll()
{
    UNPROTECT_PTR(m_factory_method_call);
    UNPROTECT_PTR(m_rho);
}

bool ClassTests::isWritable(SEXP instance)
{
    if (DATAPTR_OR_NULL(instance) == nullptr) {
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

TestResult ClassTests::testSetElt()
{
    INIT_TEST;
    SKIP_IF_NOT( TYPEOF(instance) == INTSXP || TYPEOF(instance) == REALSXP);
    SKIP_IF_NOT( isWritable(instance));
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
    SKIP_IF_NOT( isWritable(instance));

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
    SKIP_IF_NOT( TYPEOF(instance) != STRSXP);
    SKIP_IF_NOT( isWritable(instance));
    const void *dataptr_old = DATAPTR(instance);
    const int len = LENGTH(instance);

    SKIP_IF_NOT( TYPEOF(instance) == INTSXP);

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
    SKIP_IF_NOT( isWritable(instance));

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
    SKIP_IF_NOT( isWritable(instance));
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

TestResult ClassTests::testGetOneRegion()
{
    INIT_TEST;
    SKIP_IF_NOT( TYPEOF(instance) == INTSXP);
    SKIP_IF_NOT( LENGTH(instance) > 5);
    SKIP_IF_NOT( isWritable(instance));

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
    SKIP_IF_NOT( isWritable(instance));

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
    // Tady by mela byt validni odpoved i UNKNOWN_SORTEDNESS
    if (sorted == UNKNOWN_SORTEDNESS) {
        Rprintf("%s: Is_sorted has default implementation, skipping rest of test...\n", __func__);
        return TestResult::Skip;
    }
    CHECK( sorted == KNOWN_UNSORTED);
    // TODO: This CHECK is unnecessary.
    CHECK( Tests::isBufferSorted(INTEGER(instance), LENGTH(instance), sorted));
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
    SKIP_IF_NOT( isWritable(instance));

    // Set first few elements "randomly" so we get KNOWN_UNSORTED.
    SET_STRING_ELT(instance, 0, Rf_mkChar("xxx"));
    SET_STRING_ELT(instance, 1, Rf_mkChar("aaa"));
    SET_STRING_ELT(instance, 2, Rf_mkChar("zzz"));

    int sorted = STRING_IS_SORTED(instance);
    if (sorted == UNKNOWN_SORTEDNESS) {
        Rprintf("%s: Is_sorted has default implementation, skipping rest of test...\n", __func__);
        return TestResult::Skip;
    }
    CHECK( sorted == KNOWN_UNSORTED);
    FINISH_TEST;
}

TestResult ClassTests::testIsSortedIncreasing()
{
    INIT_TEST;
    SKIP_IF_NOT( TYPEOF(instance) == INTSXP || TYPEOF(instance) == REALSXP);
    SKIP_IF_NOT( isWritable(instance));

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
        return TestResult::Skip;
    }
    CHECK( sorted_mode == SORTED_INCR);
    CHECK( Tests::isBufferSorted(INTEGER(instance), LENGTH(instance), sorted_mode));
    FINISH_TEST;
}

TestResult ClassTests::testSumIntWithPreset()
{
    INIT_TEST;
    SKIP_IF_NOT( LENGTH(instance) > 10);
    SKIP_IF_NOT( TYPEOF(instance) == INTSXP);
    SKIP_IF_NOT( isWritable(instance));

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
    SKIP_IF_NOT( isWritable(instance));

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
    SKIP_IF_NOT( isWritable(instance));

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
