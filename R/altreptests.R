
is.altrep <- function(x) {
    .Call("is_altrep", x)
}

altrep_class_tests <- function(factory_method) {
    if (!is.language(factory_method)) {
        stop("Parameter factory method is not language")
    }
    .Call("class_tests", factory_method, parent.frame())
}

altrep_framework_tests <- function() {
    .Call("framework_tests")
}

simple_class.new <- function() {
    .Call("simple_class_new")
}

altrep_simple_string_class.new <- function() {
    .Call("simple_string_class_new")
}
