#include <algorithm> // std::copy
#include <iostream> // std::cin, std::cout
#include <iterator> // std::istream, std::ostream_iterator, std::inserter
#include <vector> // std::vector

template <typename T>
using it_type = typename std::iterator_traits<T>::value_type;

template <typename I, typename O>
auto
reverse_function(I first, I last, O out) {
    auto v = std::vector<it_type<I>>{};
    std::copy(first, last, std::back_inserter(v));
    std::copy(v.rbegin(), v.rend(), out);
}

int
main(int, char *[]) {
    auto in = std::istream_iterator<int>{std::cin};
    [[maybe_unused]] auto N = *in++; // get number of ints, part of the input
    reverse_function(
        in,
        std::istream_iterator<int>{},
        std::ostream_iterator<int>{std::cout, " "} // delim is " "
    );
    return 0;
}
