#include <R.h>
#include <Rinternals.h>
#include "tests_common.h"

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
        Rprintf("Running %s\n", test->name);
        test->func();
        Rprintf("Finished %s\n", test->name);
        test++;
    }
}