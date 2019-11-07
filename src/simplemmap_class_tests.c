#include "simplemmap_class_tests.h"
#include "simplemmap.h"
#include "class_tests.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

static const char *fname = "some_tmp_file_name";
static const int file_size = 25;
static void create_file();
static void delete_file();

void simplemmap_class_tests_init(DllInfo *dll)
{
    simplemmap_init_classes(dll);
    create_file();
}

void simplemmap_class_tests_deinit()
{
    delete_file();
}

SEXP simplemmap_class_tests_run()
{
    SEXP instance = simplemmap_new_instance();
    SEXP succ = class_tests_run(instance);
    return succ;
}


static void create_file()
{
    int fd = creat(fname, S_IWUSR | S_IRUSR | S_IROTH | S_IWOTH);
    if (fd < 0) {
        const char *err_msg = strerror(errno);
        error("Error in creat: %s\n", err_msg);
    }

    int buff[file_size];
    memset(buff, file_size, 42);

    if (write(fd, buff, file_size) < 0) {
        const char *err_msg = strerror(errno);
        error("write failed: %s\n", err_msg);
    }
    close(fd);
}

static void delete_file()
{
    if (remove(fname) < 0) {
        const char *err_msg = strerror(errno);
        error("remove failed: %s\n", err_msg);
    }
}
