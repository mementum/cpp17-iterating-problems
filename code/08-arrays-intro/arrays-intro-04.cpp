#include <algorithm> // std::copy, std::copy_n
#include <iostream> // std::cin, std::cout
#include <iterator> // std::istream_iterator
#include <memory> // std::make_unique

template<typename T>
T *abegin(const std::unique_ptr<T[]> &p) {
    return p.get();
}

int
main(int, char *[]) {
    auto in = std::istream_iterator<int>(std::cin);
    auto in_last = std::istream_iterator<int>();
    auto out = std::ostream_iterator<int>(std::cout, " ");  // delim is " "

    // gather array size and prepare it
    auto N = *in++;
    auto arr = std::make_unique<int[]>(N); // smart pointer

    // solve
    std::copy(in, in_last, abegin(arr));
    std::copy(
        std::make_reverse_iterator(std::next(abegin(arr), N)),
        std::make_reverse_iterator(abegin(arr)),
        out
    );

    return 0;
}
