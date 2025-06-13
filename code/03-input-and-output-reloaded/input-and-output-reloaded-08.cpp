#include <iostream> // std:cin/cout
#include <iterator> // std::istream_iterator/ostream_iterator
#include <numeric> // std::accumulate
#include <type_traits> // std::iterator_traits/enable_if/is_base_of

template <typename, typename = void>
struct is_it : std::false_type {};

template <typename T>
struct is_it<T, std::void_t<typename std::iterator_traits<T>::iterator_category>>
    : std::true_type {};

template <typename ...T>
using are_it = std::conjunction<is_it<T>...>;

template <typename, typename = void>
struct is_input : std::false_type {};

template <typename I>
struct is_input<I, std::void_t<std::is_base_of<
        std::input_iterator_tag,
        typename std::iterator_traits<I>::iterator_category>>>
    : std::true_type {};

template <typename, typename = void>
struct is_output : std::false_type {};

template <typename O>
struct is_output<O, std::void_t<std::is_base_of<
        std::input_iterator_tag,
        typename std::iterator_traits<O>::iterator_category>>>
    : std::true_type {};

template<typename I, typename O>
using io_iterators = std::conjunction<is_input<I>, is_output<O>>;

template<typename I, typename O>
constexpr bool io_iterators_v = io_iterators<I, O>::value;

template <typename T>
using it_type = decltype(*std::declval<T>());

template<typename I, typename O>
using enable_if_io = std::enable_if_t<io_iterators_v<I, O>, it_type<I>>;

template <typename I, typename O, typename I_Type = enable_if_io<I, O>>
auto
input_and_output(I first, I last, O out, I_Type init = {}) {
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
