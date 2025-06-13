#include <iostream> // std:cin/cout
#include <iterator> // std::istream_iterator/ostream_iterator
#include <numeric> // std::accumulate
#include <type_traits> // std::iterator_traits/enable_if/is_base_of

template <typename T, typename Tag>
constexpr bool is_it_tag_v = std::is_base_of_v<
    Tag, typename std::iterator_traits<T>::iterator_category>;

template <typename I>
constexpr bool is_input_v = is_it_tag_v<I, std::input_iterator_tag>;

template <typename O>
constexpr bool is_output_v = is_it_tag_v<O, std::output_iterator_tag>;

template<typename I, typename O>
constexpr bool io_iterators_v = is_input_v<I> && is_output_v<O>;

template <typename T>
using it_type = decltype(*std::declval<T>());

template<typename I, typename O>
using I_Type = std::enable_if_t<io_iterators_v<I, O>, it_type<I>>;

template <typename I, typename O>
auto
input_and_output(I first, I last, O out, I_Type<I, O> init = {}) {
    *out++ = std::accumulate(first, last, init);
}

int
main(int, char *[]) {
    using prob_type = int;
    input_and_output(
        std::istream_iterator<prob_type>{std::cin},
        std::istream_iterator<prob_type>{},
        std::ostream_iterator<prob_type>{std::cout}
    );
    return 0;
}
