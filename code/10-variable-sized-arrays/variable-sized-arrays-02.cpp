#include <algorithm> // std::cin, std::cout
#include <iostream> // std::cin, std::cout
#include <iterator> // std::istream, std::ostream_iterator, std::inserter
#include <vector> // std::vector

int
main(int, char *[]) {
    // prepare iterators for input/output
    auto in = std::istream_iterator<int>{std::cin};
    auto out = std::ostream_iterator<int>{std::cout, "\n"};
    // problem parameters (const and from input)
    auto n = *in++, q = *in++; // number of arrays, number of queries
    auto vmatrix = std::vector<std::vector<int>>{}; // create
    for(; n--; in++) { // resync "in" iterator after copy action
        auto &vec = *vmatrix.emplace(vmatrix.end());
        std::copy_n(in, *in++, std::back_inserter(vec)); // gather input
    }
    // run the queries
    for(; q--; *out = vmatrix[*in++][*in++]);
    return 0;
}
