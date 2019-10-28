#include <R.h>
#include <Rinternals.h>

#define CHECK(cond) \
    if (!(cond)) \
        _error(__FILE__, __LINE__, NULL, 0)

#define CHECK_MSG(cond, msg) \
    if (!(cond)) \
        _error(__FILE__, __LINE__, msg, 0)

#define ASSERT(cond) \
    if (!(cond)) \
        _error(__FILE__, __LINE__, NULL, 1)

typedef struct {
    char *name;
    void (*func)(void);
} test_t;


inline void _error(const char *file, int line, const char *msg, int fail)
{
    Rprintf("Error at %s:%d", file, line);
    if (msg)
        Rprintf(", msg=%s", msg);
    Rprintf("\n");
    if (fail) {
        error("Failure, exiting...");
    }
}

inline void run_all_tests(const test_t *tests)
{
    const test_t *test = tests;
    while (test != NULL && test->name != NULL) {
        Rprintf("Running %s\n", test->name);
        test->func();
        Rprintf("Finished %s\n", test->name);
        test++;
    }
}
