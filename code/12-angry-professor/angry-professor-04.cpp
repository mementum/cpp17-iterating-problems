#include <array> // std::array
#include <algorithm> // std::copy_n
#include <iostream> // std::cout/cin
#include <iterator> // std::istream/ostream_iterator, std::back_inserter
#include <vector> // std::vector

// get iterator type
template <typename I>
using it_type = typename std::iterator_traits<I>::value_type;

// check if iterator is input iterator
template <typename T, typename Tag>
constexpr bool is_it_tag_v =
    std::is_base_of_v<Tag, typename std::iterator_traits<T>::iterator_category>;

template <typename I>
constexpr bool is_input_v = is_it_tag_v<I, std::input_iterator_tag>;

// check if iterator is std::istream_iterator
template <template <typename> class B, typename I>
constexpr bool is_base_of_iter_v = std::is_base_of_v<B<it_type<I>>, I>;

template <typename I>
constexpr bool is_istream_iter_v = is_base_of_iter_v<std::istream_iterator, I>;

// check function invocation and return type
template<typename I, typename F>
constexpr bool is_fnk =
    std::is_same_v<bool, std::invoke_result_t<F, it_type<I>>>;

// enabling check for custom count algorithm
template<typename I, typename F>
using enable_if_iter_fnk = std::enable_if_t<is_input_v<I> and is_fnk<I, F>>;

// custom count algorithm
template <typename I, typename F, typename = enable_if_iter_fnk<I, F>>
auto
count_n_until_k_if(I first, int n, int k, F fnk) {
    auto count = 0;
    for (; n-- and count != k; ++first)
        count += fnk(*first);

    if constexpr (is_istream_iter_v<I>)
        for(; n >= 0; --n, ++first); // consume "n" elements from istream_iter

    return count;
}

// Main
int
main(int, char *[]) {
    auto in = std::istream_iterator<int>{std::cin}; // input iterator
    auto in_last = std::istream_iterator<int>{}; // input iterator end
    auto out = std::ostream_iterator<std::string>{std::cout, "\n"}; // out iter
    constexpr auto canceled = std::array{"NO", "YES"};
    auto fearly = [](const auto &x) { return x <= 0; };
    [[maybe_unused]] auto t = *in++; // number of testcases (unused)
    for(; in != in_last; ++in) { // resync "in" after copy_n operation
        const auto n = *in++, k = *in++; // students and threshold
#ifdef CASE1
        auto c = std::vector<int>{}; // storage
        std::copy_n(in, n, std::back_inserter(c)); // copy input
        auto &&first = c.begin();
#else
        auto first = in;
#endif
        const auto students = count_n_until_k_if(first, n, k, fearly);
        *out++ = canceled[students < k];
    }
}
