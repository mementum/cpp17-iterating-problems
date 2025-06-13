#include <iostream> // std:cin/cout
#include <iterator> // std::istream_iterator/ostream_iterator
#include <numeric> // std::accumulate
#include <type_traits> // std::iterator_traits/enable_if/is_base_of

template <typename I, typename O, typename T>
std::enable_if_t<
    // enable if condition
    std::is_base_of_v<std::input_iterator_tag,
        typename std::iterator_traits<I>::iterator_category> &&
    std::is_base_of_v<std::output_iterator_tag,
        typename std::iterator_traits<O>::iterator_category>
    >
input_and_output(I first, I last, O out, T init = T{}) {
    *out++ = std::accumulate(first, last, init);
}

int
main(int, char *[]) {
    using prob_type = int;
    input_and_output(
        std::istream_iterator<prob_type>{std::cin},
        std::istream_iterator<prob_type>{},
        std::ostream_iterator<prob_type>{std::cout},
        0
    );
    return 0;
}
