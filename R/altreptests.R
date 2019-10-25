
altrep_tests <- function(class_descriptor, instance_data1=NULL, instance_data2=NULL) {
    .Call("run_tests_for_descriptor", class_descriptor, instance_data1, instance_data2)
}
