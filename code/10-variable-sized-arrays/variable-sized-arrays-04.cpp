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
    template <typename I>
    struct ReIterator {
        using pos_t = std::pair<int, int>;

        template <typename Pair>
        struct PosT {
            typename Pair::first_type first;
            typename Pair::second_type second;

            PosT(const Pair& pos) : first{pos.first}, second{pos.second} {};

            template <typename P>
            auto operator ==(const P &) const { return false; }
            auto operator ==(const Pair &p) const {
                return p.first == first and p.second == second;
            }
        };
        using Pos = PosT<pos_t>;

        // Iterator tags
        using iterator_category = std::random_access_iterator_tag;
        using difference_type   = Pos;
        using value_type        = T;
        using reference         = value_type &; // usually value_type &
        using pointer           = value_type *; // usually value_type *

        I m_it;
        Pos m_pos = pos_t{0, 0};

        ReIterator(I it) : m_it{it} {}

        auto &operator ++() { return *this; } // nop - needed by std::advance
        auto &operator --() { return *this; } // nop - needed by std::advance

        auto &operator +=(const Pos &pos) {
            m_pos = pos;
            return *this;
        }

        constexpr auto operator *() const {
            using InnerT = typename std::iterator_traits<I>::value_type;
            auto inner = *std::next(m_it, m_pos.first);
            // constexpr chooses the right iterator to start with
            if constexpr (has_push_front_v<InnerT>)
                return *std::next(inner.rbegin(), m_pos.second);
            else if constexpr (has_push_back_v<InnerT> or has_insert_v<InnerT>)
                return *std::next(inner.begin(), m_pos.second);
            // Unreachable SFINAE has checked before
        }

        auto operator ==(const ReIterator& o) const { return m_it == o.m_it; }
        auto operator !=(const ReIterator& o) const { return m_it != o.m_it; }
    };

    using Inner = ContIn<T>; // inner array
    using Outer = ContOut<Inner>; // outer array
    Outer m_c; // outer container, keeps the other arrays in pace

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

    constexpr auto get_begin_end(bool begin) const {
        // constexpr chooses the right iterator to start with
        if constexpr (has_push_front_v<Outer>)
            return begin ? ReIterator(m_c.rbegin()) : ReIterator(m_c.rend());
        else if constexpr (has_push_back_v<Outer> or has_insert_v<Outer>)
            return begin ? ReIterator(m_c.begin()) : ReIterator(m_c.end());
        // Unreachable SFINAE has checked before
    }

public:
    using value_type = T; // needed to support std::back_inserter

    auto begin() const { return get_begin_end(true); }
    auto end() const { return get_begin_end(false); }

    auto &operator ++() {
        *get_inserter(m_c)++ = Inner{};
        return *this;
    }

    constexpr auto push_back(const T &t) {
        if constexpr (has_push_front_v<Outer>)
            return *get_inserter(*(--m_c.rend())) = t;
        else if constexpr (has_push_back_v<Outer> or has_insert_v<Outer>)
            return *get_inserter(*(--m_c.end())) = t;
        // Unreachable SFINAE has checked before
    }
};

template <typename I>
auto
variable_sized_arrays(I &in, size_t n) {
#ifdef CASE1
    auto vsa = VariableSizedArrays<std::deque, std::deque>{};
#elif defined CASE2
    auto vsa = VariableSizedArrays<std::deque, std::vector>{};
#elif defined CASE3
    auto vsa = VariableSizedArrays<std::vector, std::deque>{};
#else
    auto vsa = VariableSizedArrays{};
#endif
    for(; n--; in++) // resync "in" iterator after copy action
        std::copy_n(in, *in++, std::back_inserter(++vsa)); // *in++ = k

    return vsa;
}

int
main(int, char *[]) {
    // prepare iterators for input/output
    auto in = std::istream_iterator<int>{std::cin};
    auto in_last = std::istream_iterator<int>{};
    auto out = std::ostream_iterator<int>{std::cout, "\n"};
    // problem parameters (const and from input)
    auto n = *in++;
    [[maybe_unused]] auto q = *in++; // num arrays, num queries
    auto &&vsa = variable_sized_arrays(in, n); // get matrix
    while(in != in_last) // run queries until input is exhausted
        *out++ = *std::next(vsa.begin(), {{*in++, *in++}}); // *in++ => i, j
    return 0;
}
