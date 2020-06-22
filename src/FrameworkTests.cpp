#include "FrameworkTests.hpp"

const std::vector< Test> FrameworkTests::tests = {
    {"test_altrep_inheritance", testAltrepInheritance},
    {"test_struct_header", testStructHeader},
    {"test_instance_data", testInstanceData},
    {"test_modify_instance_data", testModifyInstanceData},
    {"test_set_instance_data", testSetInstanceData},
    {"test_redefine_method", testRedefineMethod}
};
R_altrep_class_t FrameworkTests::simple_descr;

static void * dummy_Dataptr(SEXP instance, Rboolean writeable) {
    if (R_altrep_data1(instance) != R_NilValue) {
        return DATAPTR(R_altrep_data1(instance));
    }
    else {
        return nullptr;
    }
}

static R_xlen_t dummy_Length(SEXP instance) {
    if (R_altrep_data1(instance) != R_NilValue) {
        return LENGTH(R_altrep_data1(instance));
    }
    else {
        return 1;
    }
}

void FrameworkTests::init(DllInfo *dll)
{
    simple_descr = R_make_altinteger_class("DummyClass", "AltrepTests", dll);
    R_set_altrep_Length_method(simple_descr, dummy_Length);
    R_set_altvec_Dataptr_method(simple_descr, dummy_Dataptr);
}

SEXP FrameworkTests::run()
{
    bool succ = Tests::runAll(tests);
    return ScalarLogical(succ);
}

TestResult FrameworkTests::testAltrepInheritance()
{
    INIT_TEST;
    SEXP instance = R_new_altrep(simple_descr, R_NilValue, R_NilValue);
    CHECK( R_altrep_inherits(instance, simple_descr));
    FINISH_TEST;
}


TestResult FrameworkTests::testStructHeader()
{
    INIT_TEST;
    SEXP instance = R_new_altrep(simple_descr, R_NilValue, R_NilValue);
    CHECK_MSG( ALTREP(instance), "Instance has not ALTREP bit set");
    FINISH_TEST;
}

TestResult FrameworkTests::testInstanceData()
{
    INIT_TEST;
    // data1 = [42], data2 = NA
	SEXP expected_instance_data1 = PROTECT(Rf_allocVector(INTSXP, 1));
	int *data_ptr = INTEGER(expected_instance_data1);
	*data_ptr = 42;
	SEXP expected_instance_data2 = R_NilValue;

    SEXP instance = R_new_altrep(simple_descr, expected_instance_data1, expected_instance_data2);

	int default_flags = 16;
	CHECK( R_compute_identical(R_altrep_data1(instance), expected_instance_data1, default_flags));
	CHECK( R_compute_identical(R_altrep_data2(instance), expected_instance_data2, default_flags));
    UNPROTECT(1);
    FINISH_TEST;
}

TestResult FrameworkTests::testModifyInstanceData()
{
    INIT_TEST;
    // data1 will be VecInt[], ale will be set to VecInt[42]
    SEXP int_vec = PROTECT(Rf_allocVector(INTSXP, 1));
    SEXP instance = PROTECT(R_new_altrep(simple_descr, int_vec, R_NilValue));

    SET_INTEGER_ELT(R_altrep_data1(instance), 0, 42);

    SEXP actual_data1 = R_altrep_data1(instance);
    CHECK( Rf_isInteger(actual_data1));
    CHECK( LENGTH(actual_data1) == 1);
    CHECK( INTEGER_ELT(actual_data1, 0) == 42);

    UNPROTECT(2);
    FINISH_TEST;
}

TestResult FrameworkTests::testSetInstanceData()
{
    INIT_TEST;
    SEXP instance = PROTECT(R_new_altrep(simple_descr, R_NilValue, R_NilValue));
    SEXP int_vec_1 = PROTECT(ScalarInteger(42));
    SEXP int_vec_2 = PROTECT(ScalarInteger(23));
    R_set_altrep_data1(instance, int_vec_1);
    R_set_altrep_data2(instance, int_vec_2);

    int default_flags = 16;
    CHECK( R_compute_identical(R_altrep_data1(instance), int_vec_1, default_flags));
    CHECK( R_compute_identical(R_altrep_data2(instance), int_vec_2, default_flags));

    UNPROTECT(3);
    FINISH_TEST;
}


/**
 * Tests that one DATAPTR call in AST should be able to return two different values.
 */
TestResult FrameworkTests::testDifferentDataptrValue()
{
    INIT_TEST;

    SEXP data1 = ScalarInteger(223);
    SEXP data2 = ScalarInteger(42);
    SEXP instance1 = R_new_altrep(simple_descr, data1, R_NilValue);
    SEXP instance2 = R_new_altrep(simple_descr, data2, R_NilValue);

    void *first_dataptr = nullptr;
    void *second_dataptr = nullptr;
    for (int i = 0; i < 2; i++) {
        SEXP instance = i == 0 ? instance1 : instance2;
        // This DATAPTR call returns two different values in two different invocations,
        // but from AST perspective this is still the same DATAPTR call.
        void *dataptr = DATAPTR(instance);
        first_dataptr = i == 0 ? dataptr : nullptr;
        second_dataptr = i == 1 ? dataptr : nullptr;
    }
    CHECK( first_dataptr != second_dataptr);

    FINISH_TEST;
}


static int data[5] = {1, 2, 3, 4, 5};

static R_xlen_t temp_Length(SEXP instance) {
    return 5;
}

static void * temp_Dataptr(SEXP instance, Rboolean writeable) {
    return data;
}

/**
 * In GNU-R whenever a class definition is changed eg. a pointer to one of its' methods,
 * the effects can be seen immediately by all instances. This is because in GNU-R, a
 * class definition is represented by a raw vector containing pointers to methods, and an
 * instance has a pointer to this raw vector.
 * 
 * We do not expect this feature to be exploited often, however for compatibility reasons
 * we have to test it.
 */
TestResult FrameworkTests::testRedefineMethod()
{
    INIT_TEST;
    R_set_altrep_Length_method(simple_descr, &temp_Length);
    R_set_altvec_Dataptr_method(simple_descr, &temp_Dataptr);
    SEXP instance = R_new_altrep(simple_descr, R_NilValue, R_NilValue);
    CHECK( 5 == LENGTH(instance));
    CHECK( data == DATAPTR(instance));

    // Reset the class to its previous state.
    R_set_altrep_Length_method(simple_descr, &dummy_Length);
    R_set_altvec_Dataptr_method(simple_descr, &dummy_Dataptr);
    SEXP new_instance = R_new_altrep(simple_descr, R_NilValue, R_NilValue);
    CHECK( 0 == LENGTH(new_instance));
    CHECK( nullptr == DATAPTR(new_instance));
    FINISH_TEST;
}