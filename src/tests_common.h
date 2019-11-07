#ifndef TESTS_COMMON_H_
#define TESTS_COMMON_H_

#include <R_ext/Altrep.h>

Rboolean DEBUG;

#define CHECK(cond) \
    if (!(cond)) \
        _error(__FILE__, __LINE__, NULL, 0)

#define CHECK_MSG(cond, msg) \
    if (!(cond)) \
        _error(__FILE__, __LINE__, msg, 0)

#define ASSERT(cond) \
    if (!(cond)) \
        _error(__FILE__, __LINE__, NULL, 1)

#define PREFIX "[NATIVE]: "
char *__msg;
#define LOG(fmt, ...) \
    if (DEBUG) { \
        __msg = (char *)malloc(strlen(PREFIX) + strlen(fmt) + 1); \
        strcpy(__msg, PREFIX); \
        strcpy(__msg + strlen(PREFIX), fmt); \
        Rprintf(__msg, ##__VA_ARGS__);\
        free(__msg); \
    }

typedef struct {
    char *name;
    void (*func)(void);
} test_t;

typedef struct {
    R_altrep_class_t class_descriptor;
    SEXP data1;
    SEXP data2;
} descr_with_data_t;


void _error(const char *file, int line, const char *msg, int fail);
Rboolean run_all_tests(const test_t *tests);
void print_buffer(const int *buffer, size_t size);
void check_buffers(const int *expected, const int *actual, size_t size);
SEXP wrapper_new_altrep(R_altrep_class_t class_descriptor, SEXP data1, SEXP data2);
SEXP wrapper_altrep_data1(SEXP instance);
SEXP wrapper_altrep_data2(SEXP instance);
void * wrapper_dataptr(SEXP instance);
R_xlen_t wrapper_length(SEXP instance);

#endif
