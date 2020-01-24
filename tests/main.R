stopifnot(require("altreptests"))

some_test_failed <- FALSE

run_tests <- function(tests) {
    for (test in tests) {
        func_name <- test[[1]]
        func <- test[[2]]
        cat("[R] =============================================================================\n")
        cat("[R] Running ", func_name, "\n")
        succ <- func()
        if (!succ) {
            cat("[R] ", func_name, " FAILED\n")
            some_test_failed <- TRUE
        }
        else {
            cat("[R] ", func_name, "SUCCEEDED\n")
        }
        cat("=============================================================================\n")
    }

    if (some_test_failed) {
        stop("[R] Some test failed")
        quit(save="no", status=1)
    }
}

test_framework <- function() {
    altrep_framework_tests()
}

test_simple_class <- function() {
    altrep_simple_class_tests()
}

test_simple_string_class <- function() {
    altrep_simple_string_class_tests()
}

test_simplemmap <- function() {
    stopifnot(require("simplemmap"))
    FILE_SIZE <- 20
    fname <- tempfile()
    data <- rep.int(42, FILE_SIZE)
    writeBin(data, fname)
    mmap_instance <- mmap(fname, type="int", ptrOK=TRUE, wrtOK=TRUE, serOK=FALSE)

    succ <- altrep_class_tests(mmap_instance)

    munmap(mmap_instance)
    if (!file.remove(fname)) {
        warning("File ", fname, " remove failed")
    }
    return (succ)
}

test_vroom <- function() {
    # TODO: ....
    stopifnot(require("vroom"))
}

ONLY_ONE_TEST <- FALSE
TESTS <- list(
    list("test_framework (native)", test_framework),
    list("test_simple_class (native)", test_simple_class),
    list("test_simplemmap", test_simplemmap),
    list("test_simple_string_class (native)", test_simple_string_class)
)
ONE_TEST <- list(list("test_simplemmap", test_simplemmap))



if (ONLY_ONE_TEST) {
    run_tests(ONE_TEST)
} else {
    run_tests(TESTS)
}
