
altrep_class_tests <- function(instance) {
    .Call("class_tests", instance)
}

altrep_framework_tests <- function() {
    .Call("framework_tests")
}

altrep_simple_class_tests <- function() {
    .Call("simple_class_tests")
}

altrep_simple_string_class_tests <- function() {
    .Call("simple_string_class_tests")
}

altrep_simple_class_ctor <- function() {
    .Call("simple_class_ctor")
}

altrep_simple_string_class_ctor <- function() {
    .Call("simple_string_class_ctor")
}
