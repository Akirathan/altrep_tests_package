#include <R.h>
#include <Rinternals.h>
#include "tests_common.h"
#include <stdarg.h>

void _error(const char *file, int line, const char *msg, int fail)
{
    Rprintf("Error at %s:%d", file, line);
    if (msg)
        Rprintf(", msg=%s", msg);
    Rprintf("\n");
    if (fail) {
        error("Failure, exiting...");
    }
}

void run_all_tests(const test_t *tests)
{
    const test_t *test = tests;
    while (test != NULL && test->name != NULL) {
        _log("Running %s\n", test->name);
        test->func();
        _log("Finished %s\n", test->name);
        test++;
    }
}

void _log(const char *fmt, ...)
{
    const char prefix[] = "[NATIVE]: ";

    // msg = prefix + fmt
    char *msg = (char *)malloc(strlen(prefix) + strlen(fmt));
    strcpy(msg, prefix);
    strcpy(msg + strlen(prefix), fmt);

    va_list args;
    va_start(args, fmt);
    Rvprintf(msg, args);
    va_end(args);

    free(msg);
}

SEXP wrapper_new_altrep(R_altrep_class_t class_descriptor, SEXP data1, SEXP data2)
{
    SEXP instance = R_new_altrep(class_descriptor, data1, data2);
    if (DEBUG) {
        _log("New instance=%d(0x%x)\n", instance, instance);
    }
    return instance;
}

SEXP wrapper_altrep_data1(SEXP instance)
{
    if (DEBUG) {
        _log("R_altrep_data1(instance=%d(0x%x))", instance, instance);
    }
    SEXP data1 = R_altrep_data1(instance);
    if (DEBUG) {
        _log("  -> data1=%d(0x%x)\n", data1, data1);
    }
    return data1;
}

SEXP wrapper_altrep_data2(SEXP instance)
{
    if (DEBUG) {
        _log("R_altrep_data2(instance=%d(0x%x))", instance, instance);
    }
    SEXP data2 = R_altrep_data2(instance);
    if (DEBUG) {
        _log("  -> data2=%d(0x%x)\n", data2, data2);
    }
    return data2;
}

void * wrapper_dataptr(SEXP instance)
{
    if (DEBUG) {
        _log("DATAPTR(instance=%d(0x%x))", instance, instance);
    }
    void *ptr = DATAPTR(instance);
    if (DEBUG) {
        _log("  -> ptr=%d(0x%x)\n", ptr, ptr);
    }
    return ptr;
}

R_xlen_t wrapper_length(SEXP instance)
{
    if (DEBUG) {
        _log("LENGTH(instance=%d(0x%x))", instance, instance);
    }
    R_xlen_t len = LENGTH(instance);
    if (DEBUG) {
        _log("  -> length=%d\n", len);
    }
    return len;
}
