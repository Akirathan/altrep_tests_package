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
        cat("=============================================================================\n\n")
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
    altrep_class_tests(quote(simple_class.new()))
}

test_simple_string_class <- function() {
    altrep_simple_string_class_tests()
}


test_compactseq <- function() {
    factory_method <- quote(1:10)
    altrep_class_tests(factory_method)
}

test_wrapper_object <- function() {
    x <- as.integer( rnorm(20, mean=150, sd=150))
    # sort(x) creates "Wrapper object"
    altrep_class_tests(quote(sort(x)))
}

test_deferred_string_conversion <- function() {
    x <- 1:20
    # `as.character(x)` creates deferred string conversion altrep object
    altrep_class_tests(quote(as.character(x)))
}

test_simplemmap <- function() {
    stopifnot(require("simplemmap"))
    FILE_SIZE <- 20
    fname <- tempfile()
    data <- as.integer( rnorm(FILE_SIZE, mean=150, sd=150))
    writeBin(data, fname)

    factory_method <- quote(mmap(fname, type="int", ptrOK=TRUE, wrtOK=TRUE, serOK=FALSE))
    succ <- altrep_class_tests(factory_method)

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
    #list("test_simple_string_class (native)", test_simple_string_class)

    # Tests altrep classes in Base-R
    list("test_compactseq", test_compactseq),
    list("test_wrapper_object", test_wrapper_object),
    list("test_deferred_string_conversion", test_deferred_string_conversion),

    # Test some packages that use altrep classes
    list("test_simplemmap", test_simplemmap)
)
ONE_TEST <- list(list("mytry", test_compactseq))



if (ONLY_ONE_TEST) {
    run_tests(ONE_TEST)
} else {
    run_tests(TESTS)
}
