stopifnot(require(altreptests))

run_tests <- function(tests) {
    for (test in tests) {
        func_name <- test[[1]]
        func <- test[[2]]
        cat("=============================================================================\n")
        cat("Running ", func_name, "\n")
        func()
        cat("Finished ", func_name, "\n")
        cat("=============================================================================\n")
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
    list("test_framework", test_framework),
    list("test_simple_class", test_simple_class)
)


run_tests(TESTS)
