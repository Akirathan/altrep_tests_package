#include <R.h>
#include <Rinternals.h>
#include <R_ext/Altrep.h>

#define VEC_LEN 5

#define CHECK(cond) \
    if (!(cond)) \
        _error(__FILE__, __LINE__, NULL)

#define CHECK_MSG(cond, msg) \
    if (!(cond)) \
        _error(__FILE__, __LINE__, msg)

void _error(const char *file, int line, const char *msg)
{
    Rprintf("Error at %s:%d", file, line);
    if (msg)
        Rprintf(", msg=%s", msg);
    Rprintf("\n");
}

SEXP run_all_tests();
