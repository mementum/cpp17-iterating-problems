#include <algorithm> // std::cin, std::cout
#include <deque> // std::deque
#include <iostream> // std::cin, std::cout
#include <iterator> // std::istream, std::ostream_iterator, std::inserter
#include <vector> // std::vector
#include <type_traits> // std::void_t, std::enable_if ...

// Macro for trait definitions
#define DEFINE_HAS_METHOD(method) \
template<typename, typename = void> \
constexpr bool has_##method##_v = false; \
template<typename T> \
constexpr bool has_##method##_v<T,\
    std::void_t<decltype(std::declval<T>().method())>> = true;

// Macro for trait definitions where a value_type arg is expected
#define DEFINE_HAS_METHOD_ARG(method) \
template<typename, typename = void> \
constexpr bool has_##method##_v = false; \
template<typename T> \
constexpr bool has_##method##_v<T, \
    std::void_t<decltype(std::declval<T>().method(\
        std::declval<typename T::value_type>()))>> = true;

DEFINE_HAS_METHOD(begin)
DEFINE_HAS_METHOD(end)
DEFINE_HAS_METHOD(rbegin)
DEFINE_HAS_METHOD(rend)
DEFINE_HAS_METHOD_ARG(push_back)
DEFINE_HAS_METHOD_ARG(push_front)

// has_insert
template <typename, typename = void>
constexpr bool has_insert_v = false;

template <typename C>
constexpr bool has_insert_v<
    C,
    std::void_t<
        decltype(
            std::declval<C>().insert(
                std::declval<typename C::const_iterator>(),
                std::declval<typename C::value_type>()))>> = true;

// is_container_v
template<typename C>
constexpr bool is_container_v =
    (has_push_back_v<C> and has_begin_v<C> and has_end_v<C>) or
    (has_push_front_v<C> and has_rbegin_v<C> and has_rend_v<C>) or
    (has_insert_v<C> and has_begin_v<C> and has_end_v<C>);

// enable_if
template <typename ContOut, typename ContIn>
using enable_if_containers =
    std::enable_if_t<is_container_v<ContOut> and is_container_v<ContIn>>;

// solution class
template <
    template <typename> class ContOut = std::vector,
    template <typename> class ContIn = std::vector,
    typename T = int,
    typename = enable_if_containers<ContOut<ContIn<T>>, ContIn<T>>
    >
class VariableSizedArrays {
    using Inner = ContIn<T>; // inner array
    using Outer = ContOut<Inner>; // outer array

    Outer m_c; // outer container, keeps the other arrays in pace

    template<typename C>
    auto get_iterator(const C &c, size_t s) const {
        // constexpr chooses the right iterator to start with
        if constexpr (has_push_front_v<C>)
            return std::next(c.rbegin(), s);
        else if constexpr (has_push_back_v<C> or has_insert_v<C>)
            return std::next(c.begin(), s);
        // Unreachable SFINAE has checked before
    }

    template <typename C>
    auto get_inserter(C &c) const {
        // constexpr chooses the right iterator inserter for C
        if constexpr (has_push_front_v<C>)
            return std::front_inserter(c);
        else if constexpr (has_push_back_v<C>)
            return std::back_inserter(c);
        else if constexpr (has_insert_v<C>)
            return std::inserter(c, c.end());
        // Unreachable SFINAE has checked before
    }

public:
    template <typename I>
    auto read_array(I &in, size_t n) {
        // read n values of type "S" from "in" to a container Inner
        // add the container to our outer container
        auto inner = Inner{};
        std::copy_n(in, n, get_inserter(inner));
        *get_inserter(m_c)++ = inner;
    }

    auto query(size_t j, size_t i) const {
        return *get_iterator(*get_iterator(m_c, i), j);
    }
};

// main solution
int
main(int, char *[]) {
    // prepare iterators for input/output
    auto in = std::istream_iterator<int>{std::cin};
    auto out = std::ostream_iterator<int>{std::cout, "\n"};
    // problem parameters (const and from input)
    auto n = *in++, q = *in++; // number of arrays, number of queries
    // define matrix, create and gather array elements
#ifdef CASE1
    auto vsa = VariableSizedArrays<std::deque, std::deque>{};
#else
    auto vsa = VariableSizedArrays{};
#endif
    for(; n--; in++) // resync "in" iterator after copy action
        vsa.read_array(in, *in++); // *in++ = k, number of elements
    // run the queries
    while(q--)
        *out++ = vsa.query(*in++, *in++);

    return 0;
}
