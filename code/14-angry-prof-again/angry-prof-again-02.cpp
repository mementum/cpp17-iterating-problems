#include <array> // std::array
#include <algorithm> // std::copy_n, std::partition
#include <cmath> // std::abs
#include <iostream> // std::cout/cin
#include <iterator> // std::istream/ostream_iterator, std::back_inserter
#include <vector> // std::vector
#include <type_traits> // std::void_t, std::enable_if ...

// get iterator value_type
template <typename T>
using it_type = typename std::iterator_traits<T>::value_type;

// get iterator difference type
template <typename T>
using it_category = typename std::iterator_traits<T>::iterator_category;

template <typename T>
using it_difftype = typename std::iterator_traits<T>::difference_type;

// Check if an iterator is a class/subclass of a given tag
template <typename T, typename Tag>
constexpr bool is_it_tag_v = std::is_base_of_v<Tag, it_category<T>>;

template <typename I>
constexpr bool is_input_it_v = is_it_tag_v<I, std::input_iterator_tag>;

template <typename I>
constexpr bool is_random_it_v = is_it_tag_v<I, std::random_access_iterator_tag>;

// check function invocation and return type convertibility to bool
template<typename I, typename F>
constexpr bool is_f_v =
    std::is_convertible_v<std::invoke_result_t<F, it_type<I>>, bool>;

// check if input iterator and function can be called and delivers as expected
template<typename I, typename F>
using enable_if_n_of =
    std::enable_if_t<(is_input_it_v<I> or is_random_it_v<I>) and is_f_v<I, F>>;

// find_n_if implementation
template<typename I, typename F, typename = enable_if_n_of<I, F>>
auto
find_n_if(I first, I last, ssize_t n, const F &f, bool at_least = false) {
    it_difftype<I> dist;
    if constexpr (is_random_it_v<I>)
        dist = std::abs(std::distance(first, last));

    for(; n >= at_least and first != last; ++first) {
        if constexpr (is_random_it_v<I>)
            if ((unsigned) dist-- < n)
                break;

        n -= static_cast<bool>(f(*first));
    }
    // The loop may have been interrupted early. If n is not 0, either not
    // enough items were found or too many (at least if fase) => return last
    return n ? last : first;
}

// n_of implementation
template<typename I, typename F, typename = enable_if_n_of<I, F>>
auto
n_of(I first, I last, ssize_t n, const F &f, bool at_least = false) {
    return find_n_if(first, last, n, f, at_least) != last;
}

// Main
int
main(int, char *[]) {
    auto in = std::istream_iterator<int>{std::cin}; // input iterator
    auto in_last = std::istream_iterator<int>{}; // input iterator end
    auto out = std::ostream_iterator<std::string>{std::cout, "\n"}; // out iter
    constexpr auto canceled = std::array{"NO", "YES"};
    const auto fearly = [](const auto &x) { return x <= 0; };
    [[maybe_unused]] auto t = *in++; // number of testcases (unused)
    for(; in != in_last; ++in) { // resync "in" after copy_n operation
        const auto n = *in++, k = *in++; // students and threshold
        auto c = std::vector<int>{}; // storage
        std::copy_n(in, n, std::back_inserter(c)); // copy input
        const auto early_students = n_of(c.begin(), c.end(), k, fearly, true);
        *out++ = canceled[not early_students];
    }
    return 0;
}
