#include <iostream> // std:cin/cout
#include <iterator> // std::istream_iterator/ostream_iterator
#include <numeric> // std::accumulate
#include <type_traits> // std::iterator_traits/enable_if/is_base_of

template <typename, typename = void>
struct is_it : std::false_type {};

template <typename T>
struct is_it<T, std::void_t<typename std::iterator_traits<T>::iterator_category>>
    : std::true_type {};

template <typename I, typename O,
    std::enable_if_t<is_it<I>{} && is_it<O>{}>* = nullptr>
auto
input_and_output(I first, I last, O out, decltype(*std::declval<I>()) init = {}) {
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
