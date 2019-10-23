stopifnot(require(samplepackage))

EXPECTED_LENGTH <- 5

construct <- function() {
    return (.Call("ctor", PACKAGE="samplepackage"))
}

test_length <- function() {
    vec <- construct()
    stopifnot( length(vec) == EXPECTED_LENGTH)
}

test_data <- function() {
    vec <- construct()
    for (i in EXPECTED_LENGTH) {
        stopifnot( vec[i] == 0);
    }
}

test_modification <- function() {
    vec <- construct()
    # In this location the altvec gets replaced by normal vector (materialisation occurs).
    # TODO: How to avoid materialisation in ALTREP?
    vec[2] <- 42

    vec[3] <- 23
    expected <- c (0, 42, 23, 0, 0)
    stopifnot( expected == vec)
}

run_all_tests <- function () {
    for (test_func_tuple in list(
                        list("test_length", test_length),
                        list("test_data", test_data), 
                        list("test_modification", test_modification)
                    ))
    {
        func_name <- test_func_tuple[[1]]
        func <- test_func_tuple[[2]]
        cat("=============================================================================\n")
        cat("Running ", func_name, "\n")
        func()
        cat("Finished ", func_name, "\n")
        cat("=============================================================================\n")
    }
}


run_all_tests()
