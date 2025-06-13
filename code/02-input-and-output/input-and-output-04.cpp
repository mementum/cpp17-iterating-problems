#include <iostream> // std:cin/cout
#include <iterator> // std::istream_iterator/ostream_iterator
#include <numeric> // std::accumulate

template<typename I, typename O>
auto
input_and_output(I first, I last, O out, int init = 0) {
    *out++ = std::accumulate(first, last, init);
}

int
main(int, char *[]) {
    input_and_output(
        std::istream_iterator<int>{std::cin},
        std::istream_iterator<int>{},
        std::ostream_iterator<int>{std::cout}
    );
    return 0;
}
