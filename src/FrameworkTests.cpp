#include "FrameworkTests.hpp"
#include "SimpleClass.hpp"
#include "APIWrapper.hpp"

const std::vector< Test> FrameworkTests::tests = {
    {"test_altrep_inheritance", testAltrepInheritance},
    {"test_struct_header", testStructHeader},
    {"test_instance_data", testInstanceData},
    {"test_modify_instance_data", testModifyInstanceData},
    {"test_set_instance_data", testSetInstanceData}
};
R_altrep_class_t FrameworkTests::simple_descr;


void FrameworkTests::init(DllInfo *dll)
{
    SimpleClass::init(dll);
    simple_descr = SimpleClass::getDescriptor();
}

SEXP FrameworkTests::run()
{
    bool succ = Tests::runAll(tests);
    return ScalarLogical(succ);
}

void FrameworkTests::testAltrepInheritance()
{
    SEXP instance = APIWrapper::R_new_altrep(simple_descr, R_NilValue, R_NilValue);
    CHECK( R_altrep_inherits(instance, simple_descr));
}


void FrameworkTests::testStructHeader()
{
    SEXP instance = APIWrapper::R_new_altrep(simple_descr, R_NilValue, R_NilValue);
    CHECK_MSG( ALTREP(instance), "Instance has not ALTREP bit set");
}

void FrameworkTests::testInstanceData()
{
    // data1 = [42], data2 = NA
	SEXP expected_instance_data1 = PROTECT(allocVector(INTSXP, 1));
	int *data_ptr = INTEGER(expected_instance_data1);
	*data_ptr = 42;
	SEXP expected_instance_data2 = R_NilValue;

    SEXP instance = APIWrapper::R_new_altrep(simple_descr, expected_instance_data1, expected_instance_data2);

	int default_flags = 16;
	CHECK( R_compute_identical(APIWrapper::R_altrep_data1(instance), expected_instance_data1, default_flags));
	CHECK( R_compute_identical(APIWrapper::R_altrep_data2(instance), expected_instance_data2, default_flags));
    UNPROTECT(1);
}

void FrameworkTests::testModifyInstanceData()
{
    // data1 will be VecInt[], ale will be set to VecInt[42]
    SEXP int_vec = PROTECT(allocVector(INTSXP, 1));
    SEXP instance = PROTECT(APIWrapper::R_new_altrep(simple_descr, int_vec, R_NilValue));

    SET_INTEGER_ELT(APIWrapper::R_altrep_data1(instance), 0, 42);

    SEXP actual_data1 = APIWrapper::R_altrep_data1(instance);
    CHECK( isInteger(actual_data1));
    CHECK( length(actual_data1) == 1);
    CHECK( INTEGER_ELT(actual_data1, 0) == 42);

    UNPROTECT(2);
}

void FrameworkTests::testSetInstanceData()
{
    SEXP instance = PROTECT(APIWrapper::R_new_altrep(simple_descr, R_NilValue, R_NilValue));
    SEXP int_vec = PROTECT(ScalarInteger(42));
    SEXP lgl_vec = PROTECT(ScalarLogical(1));
    R_set_altrep_data1(instance, int_vec);
    R_set_altrep_data2(instance, lgl_vec);

    int default_flags = 16;
    CHECK( R_compute_identical(APIWrapper::R_altrep_data1(instance), int_vec, default_flags));
    CHECK( R_compute_identical(APIWrapper::R_altrep_data2(instance), lgl_vec, default_flags));

    UNPROTECT(3);
}
