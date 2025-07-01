#include <algorithm> // std::copy, std::copy_n
#include <iostream> // std::cin, std::cout
#include <iterator> // std::istream_iterator
#include <memory> // std::make_unique

int
main(int, char *[]) {
    auto in = std::istream_iterator<int>{std::cin};
    auto out = std::ostream_iterator<int>{std::cout, " "};

    // gather array size and prepare it
    auto N = *in++;
    auto arr = std::make_unique<int[]>(N); // smart pointer

    // solve
    std::copy_n(in, N, arr.get());
    std::copy_n(std::make_reverse_iterator(arr.get() + N), N, out);

    return 0;
}
