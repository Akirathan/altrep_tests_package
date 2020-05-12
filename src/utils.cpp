#include "utils.hpp"

template <typename InputIt, typename Func>
static void for_each_inner(InputIt begin, InputIt end, Func func)
{
    if (begin == end) {
        return;
    }

    InputIt it = begin;
    while (it != end - 1) {
        func(*it);
        it++;
    }
}

void print_vector(const std::vector< std::string> &vec)
{
    Rprintf("[");
    for_each_inner(vec.cbegin(), vec.cend(), [](const std::string &s) { Rprintf("%s, ", s.c_str()); });
    if (!vec.empty()) {
        Rprintf("%s", vec[vec.size() - 1].c_str());
    }
    Rprintf("]");
}

void print_vector(const std::vector< int> &vec)
{
    Rprintf("[");
    for_each_inner(vec.cbegin(), vec.cend(), [](int i) { Rprintf("%d, ", i);} );
    if (!vec.empty()) {
        Rprintf("%d", vec[vec.size() - 1]);
    }
    Rprintf("]");
}
