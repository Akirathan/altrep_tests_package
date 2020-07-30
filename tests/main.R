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
    factory_method <- function() simple_class.new()
    altrep_class_tests(factory_method)
}

test_simple_string_class <- function() {
    factory_method <- function() simple_string_class.new()
    altrep_class_tests(factory_method)
}

test_real_class <- function() {
    factory_method <- function() alt_real.new()
    altrep_class_tests(factory_method)
}

#' Tests vectors (non-altrep objects)
test_int_vectors <- function() {
    altrep_class_tests(function() c(42L, 3L))
    factory_method <- function() {
        set.seed(42)
        as.integer(runif(20, min=0, max=100))
    }
    altrep_class_tests(factory_method)
}

test_real_vectors <- function() {
    altrep_class_tests(function() c(1, 2, 5, 42, 56, 13, 10, 96, 4, 4, 13, 18))
}

test_string_vectors <- function() {
    return (TRUE)
}


test_compactseq <- function() {
    altrep_class_tests(function() 1:10)
}

test_wrapper_object <- function() {
    factory_method <- function() {
        x <- as.integer( rnorm(20, mean=150, sd=150))
        # sort(x) creates "Wrapper object"
        sort(x)
    }
    altrep_class_tests(factory_method)
}

test_deferred_string_conversion <- function() {
    factory_method <- function() {
        x <- 1:10
        # `as.character(x)` creates deferred string conversion altrep object
        as.character(x)
    }
    altrep_class_tests(factory_method)
}

test_simplemmap <- function() {
    stopifnot(require("simplemmap"))
    FILE_SIZE <- 20
    fname <- tempfile()
    data <- as.integer( rnorm(FILE_SIZE, mean=150, sd=150))
    writeBin(data, fname)

    factory_method <- function() {
        mmap(fname, type="int", ptrOK=TRUE, wrtOK=TRUE, serOK=FALSE)
    }
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
    # FIXME: After recent rebase on a new model this does not work
    #list("test_simple_string_class (native)", test_simple_string_class),
    list("test_real_class", test_real_class),

    # Test non-altrep objects
    list("test_int_vectors", test_int_vectors),
    list("test_real_vectors", test_real_vectors),
    list("test_string_vectors", test_string_vectors),

    # Tests altrep classes in Base-R
    list("test_compactseq", test_compactseq),
    list("test_wrapper_object", test_wrapper_object),
    list("test_deferred_string_conversion", test_deferred_string_conversion)

    # Test some packages that use altrep classes
    # TODO: simplemmap does not work, because the package has one ALTREP method with wrong signature.
    #list("test_simplemmap", test_simplemmap)
)
ONE_TEST <- list(list("test_int_vectors", test_int_vectors))



if (ONLY_ONE_TEST) {
    run_tests(ONE_TEST)
} else {
    run_tests(TESTS)
}
