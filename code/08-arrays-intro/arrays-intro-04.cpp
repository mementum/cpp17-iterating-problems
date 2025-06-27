#include <algorithm> // std::copy, std::copy_n
#include <iostream> // std::cin, std::cout
#include <iterator> // std::istream_iterator
#include <memory> // std::make_unique

template<typename T>
T *arrnext(const std::unique_ptr<T[]> &p, size_t n = 0) {
    return p.get() + n;
}

template<typename T>
T *arrbegin(const std::unique_ptr<T[]> &p) {
    return arrnext<T>(p);
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
    std::copy(in, in_last, arrbegin(arr));
    std::copy(
        std::make_reverse_iterator(arrnext(arr, N)),
        std::make_reverse_iterator(arrbegin(arr)),
        out
    );

    return 0;
}
