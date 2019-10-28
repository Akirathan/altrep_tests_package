
altrep_class_tests <- function(class_descriptor, instance_data1=NULL, instance_data2=NULL) {
    .Call("class_tests", class_descriptor, instance_data1, instance_data2)
}

altrep_framework_tests <- function() {
    .Call("framework_tests")
}
