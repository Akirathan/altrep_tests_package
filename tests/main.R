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

test_simplemmap <- function() {
    stopifnot(require("simplemmap"))
    FILE_SIZE <- 50
    fname <- tempfile()
    data <- runif(FILE_SIZE)
    writeBin(data, fname)
    mmap_instance <- mmap(fname, type="int", ptrOK=TRUE, wrtOK=TRUE, serOK=FALSE)

    succ <- altrep_class_tests(mmap_instance)

    munmap(mmap_instance)
    if (!file.remove(fname)) {
        warning("File ", fname, " remove failed")
    }
    return (succ)
}

TESTS <- list(
    list("test_framework (native)", test_framework),
    list("test_simple_class (native)", test_simple_class),
    list("test_simplemmap", test_simplemmap)
)


run_tests(TESTS)
