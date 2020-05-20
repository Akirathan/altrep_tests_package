#include "utils.hpp"

static constexpr size_t PRINT_LIMIT = 100;

template <typename InputIt, typename Func>
static void for_each_inner(InputIt begin, InputIt end, Func func)
{
    if (begin == end) {
        return;
    }

    size_t items = 0;
    InputIt it = begin;
    while (it != end - 1 && items < PRINT_LIMIT) {
        func(*it);
        it++;
        items++;
    }
}

void print_vector(const std::vector< std::string> &vec)
{
    Rprintf("[");
    for_each_inner(vec.cbegin(), vec.cend(), [](const std::string &s) { Rprintf("%s, ", s.c_str()); });
    if (!vec.empty() && vec.size() < PRINT_LIMIT) {
        Rprintf("%s", vec[vec.size() - 1].c_str());
    }
    Rprintf("]");
}

void print_vector(const std::vector< int> &vec)
{
    Rprintf("[");
    for_each_inner(vec.cbegin(), vec.cend(), [](int i) { Rprintf("%d, ", i);} );
    if (!vec.empty() && vec.size() < PRINT_LIMIT) {
        Rprintf("%d", vec[vec.size() - 1]);
    }
    Rprintf("]");
}

void print_vector(const std::vector< double> &vec)
{
    Rprintf("[");
    for_each_inner(vec.cbegin(), vec.cend(), [](double d) { Rprintf("%f, ", d);} );
    if (!vec.empty() && vec.size() < PRINT_LIMIT) {
        Rprintf("%f", vec[vec.size() - 1]);
    }
    Rprintf("]");
}
