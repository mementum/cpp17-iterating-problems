#include <array> // std::array
#include <algorithm> // std::copy_n, std::partition
#include <iostream> // std::cout/cin
#include <iterator> // std::istream/ostream_iterator
#include <vector> // std::vector
#include <type_traits> // std::void_t, std::enable_if ...

// get container value type
template <typename C>
using cont_type = typename C::value_type;

// Macro for trait definitions where a value_type arg is expected
#define DEFINE_HAS_METHOD_V(method) \
template<typename, typename = void> \
constexpr bool has_##method##_v = false; \
template<typename T> \
constexpr bool has_##method##_v<T,\
    std::void_t<decltype(std::declval<T>().method())>> = true;

#define DEFINE_HAS_METHOD_V_ARG(method) \
template<typename, typename = void> \
constexpr bool has_##method##_v = false; \
template<typename T> \
constexpr bool has_##method##_v<T, \
    std::void_t<decltype(std::declval<T>().method(\
        std::declval<cont_type<T>>()))>> = true;

DEFINE_HAS_METHOD_V_ARG(push_back)
DEFINE_HAS_METHOD_V_ARG(push_front)
DEFINE_HAS_METHOD_V(end)

// has_insert_v
template <typename, typename = void>
constexpr bool has_insert_v = false;

template <typename C>
constexpr bool has_insert_v<C,
    std::void_t<decltype(std::declval<C>().insert(
            std::declval<typename C::const_iterator>(),
            std::declval<cont_type<C>>()))>> = true;

// is_container_v
template <typename C>
constexpr bool is_container_v = has_push_back_v<C> or has_push_front_v<C>
    or (has_insert_v<C> and has_end_v<C>);

// check function invocation and return type
template<typename C, typename F>
constexpr bool is_f =
    std::is_convertible_v<std::invoke_result_t<F, cont_type<C>>, bool>;

// enabling check for custom count algorithm
template<typename C, typename F>
using enable_if_cont_func = std::enable_if_t<is_container_v<C> and is_f<C, F>>;

// custom back insertion iterator filtering out on false check
template <typename C, typename F, typename = enable_if_cont_func<C, F>>
struct filter_insert_iterator {
    // needed for an iterator
    using iterator_category = std::output_iterator_tag;
    using value_type = void;
    using difference_type = void;
    using pointer = void;
    using reference = void;
    using container_type = C;

    C &m_c; // wrapped container
    const F &m_f; // filtering function

    // constructor replicating the wrapped iterator's constructor
    filter_insert_iterator(C &c, const F &f) : m_c{c}, m_f{f} {}

    // no-ops because only the assignment (= operator does something)
    auto *operator ->() const { return this; } // we wouldn't need this
    auto &operator *() const { return *this; }
    auto &operator ++() const { return *this; } // ++prefix
    auto &operator ++(int) const { return *this; } // postfix++

    using ContValType = typename C::value_type;

    // Only Operational Method
    auto &operator =(const ContValType &value) const {
        if (m_f(value)) {
            if constexpr (has_push_back_v<C>)
                m_c.push_back(value);
            else if constexpr (has_push_front_v<C>)
                m_c.push_front(value);
            else if constexpr (has_insert_v<C>)
                m_c.insert(m_c.end(), value);
            // SFINAE checks prevent we get to this point
        }
        return *this;
    }
};

// helper function
template<typename C, typename F, typename = enable_if_cont_func<C, F>>
auto
filter_inserter(C &c, const F &fearly) {
    return filter_insert_iterator<C, F>(c, fearly);
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
        std::copy_n(in, n, filter_inserter(c, fearly)); // copy input
        *out++ = canceled[c.size() < static_cast<unsigned int>(k)];
    }
}
