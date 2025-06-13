#include <iostream> // std:cin/cout
#include <iterator> // std::istream_iterator/ostream_iterator
#include <numeric> // std::accumulate
#include <type_traits> // std::iterator_traits/enable_if/is_base_of

template <typename I, typename O>
std::enable_if_t<
    // enable if condition
    std::is_base_of_v<std::input_iterator_tag,
        typename std::iterator_traits<I>::iterator_category> &&
    std::is_base_of_v<std::output_iterator_tag,
        typename std::iterator_traits<O>::iterator_category>
    ,
    void> // get the type of the enable_if result
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
