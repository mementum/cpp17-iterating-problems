#include <algorithm> // std::copy
#include <array> // std::array
#include <iostream> // std::cout/cin
#include <iterator> // std::istream/ostream_iterator
#include <type_traits> // std::enable_if/is_base_of

template <typename T>
class os_iterator {
    using ositer = std::ostream_iterator<T>;
    using osdelim = std::ostream_iterator<std::string>;

    ositer m_iter; // wrapped iterator
    osdelim m_itdelim; // wrapped iterator for delimiter
    const std::string m_delim; // separator between elements
    bool m_dodelim = false; // when to start separating

public:
    // needed for an iterator - copy from wrapped iterator
    using iterator_category = typename ositer::iterator_category;
    using value_type = typename ositer::value_type;
    using difference_type = typename ositer::difference_type;
    using pointer = os_iterator *;
    using reference = os_iterator &;

    // constructor replicating the wrapped iterator's constructor
    os_iterator(std::ostream &os, const std::string &delim="") :
        m_iter{os}, m_itdelim{os}, m_delim{delim} {}

    // no-ops because only the assignment (= operator does something)
    auto *operator ->() { return this; } // we wouldn't need this
    auto &operator *() { return *this; }
    auto &operator ++() { return *this; } // ++prefix
    auto &operator ++(int) { return *this; } // postfix++

    // usual ++ postfix implementation, but we need no tmp
    // because our * and ++ operators are reall no-ops
    // auto operator ++(int) { auto tmp = *this; ++(*this); return tmp; }

    // Operation with the wrapped iterator, choosing when to
    auto &operator =(const T &outval) {
        if(m_dodelim) // output separator before the second/later elements
            *m_itdelim++ = m_delim;
        else
            m_dodelim = true;

        *m_iter++ = outval;
        return *this;
    }
};

template <typename T, typename Tag>
constexpr bool is_it_tag_v = std::is_base_of_v<
    Tag, typename std::iterator_traits<T>::iterator_category>;

template <typename I>
constexpr bool is_input_v = is_it_tag_v<I, std::input_iterator_tag>;

template <typename O>
constexpr bool is_output_v = is_it_tag_v<O, std::output_iterator_tag>;

template<typename I, typename O>
constexpr bool io_iterators_v = is_input_v<I> && is_output_v<O>;

template<typename I, typename O>
using enable_if_io = std::enable_if_t<io_iterators_v<I, O>>*;

template <typename I, typename O, enable_if_io<I, O> = nullptr>
auto
hello_world(I first, I last, O out) {
    return std::copy(first, last, out);
}

int
main(int, char *[]) {
    auto hello = std::array{"Hello", "World!"};
    auto out = os_iterator<std::string>{std::cout, ", "};
    hello_world(hello.begin(), hello.end(), out);
    return 0;
}
