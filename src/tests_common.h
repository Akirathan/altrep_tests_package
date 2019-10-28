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


void _error(const char *file, int line, const char *msg, int fail);
void run_all_tests(const test_t *tests);
