#include <algorithm> // std::copy_n
#include <functional> // std::function
#include <iostream> // std::cout/cin
#include <iterator> // std::istream/ostream_iterator
#include <numeric> // std::accumulate
#include <vector> // std::vector
#include <type_traits> // std::enable_if, std::is_integral, std::void_t

// SFINAE to check for It being an Input iterator and T and integer-like
template <typename T, typename Tag>
constexpr bool is_it_tag_v =
    std::is_base_of_v<Tag, typename std::iterator_traits<T>::iterator_category>;

template <typename I>
constexpr bool is_input_v = is_it_tag_v<I, std::input_iterator_tag>;

template <typename It, typename T>
using enable_if_iter_int =
    std::enable_if_t<is_input_v<It> and std::is_integral_v<T>>;

using FunctionMove = std::function<int(int)>;

template<typename, typename = void>
constexpr bool is_fmove_v = false;

template<typename F>
constexpr bool is_fmove_v<F,
    std::void_t<
        decltype(
            std::declval<FunctionMove>()(0) ==
            std::declval<std::invoke_result_t<F, int>>()
        )>> = true;

template<typename F>
using enable_if_fmove = std::enable_if_t<is_fmove_v<F>>;

template <
    typename It, typename T = int, typename = enable_if_iter_int<It, T>>
struct JumpingIterator {
    // Iterator tags
    using iterator_category = std::random_access_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = T;
    using reference         = value_type &;
    using pointer           = value_type *;

    const It m_itfirst;
    It m_itcur;
    FunctionMove m_fmove;

    bool m_end = false;

    template <typename F, typename = enable_if_fmove<F>>
    JumpingIterator(It first, const F &fmove)
        : m_itfirst{first}, m_itcur{first}, m_fmove(fmove) {}

    JumpingIterator() : m_end{true} {}

    auto operator ->() { return &(*m_itcur); }
    auto operator *() { return *m_itcur; }

    auto &operator ++() { // Prefix increment
        if (not m_end) {
            m_itcur = std::next(
                m_itfirst,
                m_fmove(std::distance(m_itfirst, m_itcur))
            );
            m_end = m_itfirst == m_itcur;
        }
        return *this;
    }
    // Postfix increment
    auto operator ++(int) { JumpingIterator tmp = *this; ++(*this); return tmp; }

    auto operator ==(const JumpingIterator& o) const {
        return m_end ? o.m_end : (not o.m_end and (m_itcur == o.m_itcur));
    }
    auto operator !=(const JumpingIterator& o) const { return not (*this == o); }
};

// SFINAE for the solution function
using FunctionEnergy = std::function<int(int, int)>;

template<typename, typename = void>
constexpr bool is_fenergy_v = false;

template<typename F>
constexpr bool is_fenergy_v<F,
    std::void_t<
        decltype(
            std::declval<FunctionEnergy>()(0, 0) ==
            std::declval<std::invoke_result_t<F, int, int>>()
        )>> = true;

template <typename I, typename FMove, typename FEnergy>
using enable_if_iter_fmove_fenergy = std::enable_if_t<
    is_input_v<I> and is_fmove_v<FMove> and is_fenergy_v<FEnergy>>;

template <typename I, typename FMove, typename FEnergy,
    typename = enable_if_iter_fmove_fenergy<I, FMove, FEnergy>>
auto
minus_energy(I first, FMove fmove, FEnergy fenergy) {
    return std::accumulate(
        JumpingIterator<I>(first, fmove), // first
        JumpingIterator<I>{}, // last
        0, // init
        fenergy // binary op
    );
}

///////////////////////////////////////////////////////////////////////////////
// Main
///////////////////////////////////////////////////////////////////////////////
int
main(int, char *[]) {
    constexpr auto e = 100; // starting energy level
    auto in = std::istream_iterator<int>{std::cin}; // input iterator
    auto in_last = std::istream_iterator<int>{}; // input iterator end
    auto out = std::ostream_iterator<int>{std::cout, "\n"}; // output iterator
    for(; in != in_last; in++) { // resync "in" after copy_n operation
        auto n = *in++, k = *in++; // input parameters
        auto fenergy = [](auto acc, auto x) { return acc - (1 + (x * 2)); };
        auto fmove = [&n, &k](auto x) { return (x + k) % n; };
        auto c = std::vector<bool>{}; // storage
        std::copy_n(in, n, std::back_inserter(c)); // copy input
        *out++ = e + minus_energy(c.begin(), fmove, fenergy); // solve
    }
}
