
is.altrep <- function(x) {
    .Call("is_altrep", x)
}

altrep_class_tests <- function(factory_method) {
    .Call("class_tests", factory_method, parent.frame())
}

altrep_framework_tests <- function() {
    .Call("framework_tests")
}

simple_class.new <- function() {
    .Call("simple_class_new")
}

simple_string_class.new <- function() {
    .Call("simple_string_class_new")
}

alt_real.new <- function() {
    .Call("alt_real_new")
}
