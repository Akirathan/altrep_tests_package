stopifnot(require(altreptests))

run_all_tests <- function() {
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


test_mmap <- function() {
    stopifnot(require(simplemmap))
    fname <- tempfile()
    data <- runif(20)
    writeBin(data, fname)

    mmaped_data <- mmap(fname)
    munmap(mmaped_data)
}

