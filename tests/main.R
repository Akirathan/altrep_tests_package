stopifnot(require(altreptests))

some_test_failed <- FALSE

run_tests <- function(tests) {
    for (test in tests) {
        func_name <- test[[1]]
        func <- test[[2]]
        cat("=============================================================================\n")
        cat("Running ", func_name, "\n")
        succ <- func()
        if (!succ) {
            cat(func_name, " FAILED\n")
            some_test_failed <- TRUE
        }
        else {
            cat(func_name, "SUCCEEDED\n")
        }
        cat("=============================================================================\n")
    }

    if (some_test_failed) {
        stop("Some test failed")
        quit(save="no", status=1)
    }
}

test_mmap <- function() {
    #TODO ...
    stopifnot(require(simplemmap))
    fname <- tempfile()
    data <- runif(20)
    writeBin(data, fname)

    mmaped_data <- mmap(fname)
    munmap(mmaped_data)
}

test_framework <- function() {
    altrep_framework_tests()
}

test_simple_class <- function() {
    altrep_simple_class_tests()
}

TESTS <- list(
    list("test_framework (native)", test_framework),
    list("test_simple_class (native)", test_simple_class)
)


run_tests(TESTS)
