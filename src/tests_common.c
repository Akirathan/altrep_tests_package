#include <R.h>
#include <Rinternals.h>
#include "tests_common.h"

static const test_t *_current_test;

#define FAILED_TESTS_SIZE  10
static struct {
    test_t tests[FAILED_TESTS_SIZE];
    int size;
} _failed_tests;

static void _init_failed_tests_list();
static void _add_failed_test();
static void _report_failures();


void _error(const char *file, int line, const char *msg, int fail)
{
    Rprintf("Error at %s:%d", file, line);
    if (msg)
        Rprintf(", msg=%s", msg);
    Rprintf("\n");
    if (fail) {
        error("Failure, exiting...");
    }
    _add_failed_test();
}

void run_all_tests(const test_t *tests)
{
    _init_failed_tests_list();

    const test_t *test = tests;
    while (test != NULL && test->name != NULL) {
        _current_test = test;
        Rprintf("===== Running %s ====== \n", test->name);
        test->func();
        Rprintf("===== Finished %s ===== \n", test->name);
        test++;
    }

    _report_failures();
}


SEXP wrapper_new_altrep(R_altrep_class_t class_descriptor, SEXP data1, SEXP data2)
{
    SEXP instance = R_new_altrep(class_descriptor, data1, data2);
    LOG("New instance=%d(0x%x)\n", instance, instance);
    return instance;
}

SEXP wrapper_altrep_data1(SEXP instance)
{
    LOG("Calling R_altrep_data1(instance=%d(0x%x))\n", instance, instance);
    SEXP data1 = R_altrep_data1(instance);
    LOG("Got from Java: data1=%d(0x%x)\n", data1, data1);
    return data1;
}

SEXP wrapper_altrep_data2(SEXP instance)
{
    LOG("Calling R_altrep_data2(instance=%d(0x%x))\n", instance, instance);
    SEXP data2 = R_altrep_data2(instance);
    LOG("Got from Java: data2=%d(0x%x)\n", data2, data2);
    return data2;
}

void * wrapper_dataptr(SEXP instance)
{
    LOG("Calling DATAPTR(instance=%d(0x%x))\n", instance, instance);
    void *ptr = DATAPTR(instance);
    LOG("Got from Java: ptr=%d(0x%x)\n", ptr, ptr);
    return ptr;
}

R_xlen_t wrapper_length(SEXP instance)
{
    LOG("Calling LENGTH(instance=%d(0x%x))\n", instance, instance);
    R_xlen_t len = LENGTH(instance);
    LOG("Got from Java: length=%d\n", len);
    return len;
}

static void _init_failed_tests_list()
{
    memset(&_failed_tests, 0, sizeof(_failed_tests));
}

static void _add_failed_test()
{
    if (_current_test == NULL) {
        error("_current_test should not be NULL");
    }

    if (_failed_tests.size == FAILED_TESTS_SIZE) {
        return;
    }

    // Skip duplicates.
    for (size_t i = 0; i < _failed_tests.size; i++) {
        if (strcmp(_failed_tests.tests[i].name, _current_test->name) == 0) {
            return;
        }
    }

    _failed_tests.tests[_failed_tests.size] = *_current_test;
    _failed_tests.size++;
}

static void _report_failures()
{
    if (_failed_tests.size > 0) {
        Rprintf("Failures at: ");
        for (size_t i = 0; i < _failed_tests.size; i++) {
            Rprintf("%s, ", _failed_tests.tests[i].name);
        }
        Rprintf("\n");
    }
}
