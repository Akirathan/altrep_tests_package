# The `altreptests` package
This package serves as a TCK (test compatibility kit) for an altrep instances.
It gets an altrep instance on input and runs various tests on the instance to
see whether this instance "behaves as expected".

Furthermore, this package can be used as a guide how to use altrep API.

There are some (mostly implicit, nowhere documented) **contracts** for altrep instances -
for example if an altrep instance claims that it is sorted, all its elements should
really be sorted.
These *contracts* are tested in this package.

Note that this package contains only native code.

## Usage
Call `altrep_class_tests(instance)` with `instance` being your altrep instance.
This function returns logical representing the result of the tests.
