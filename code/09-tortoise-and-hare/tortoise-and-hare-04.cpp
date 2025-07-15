#include <algorithm> // std::count
#include <functional> // std::function
#include <iostream> // std::cin, std::cout
#include <iterator> // std::istream_iterator/ostream_iterator
#include <limits> // std::numeric_limits
#include <numeric> // std::accumulate
#include <type_traits> // std::enable_if, std::is_integral, std::void_t

template <typename T = int>
struct CycleIterator {
    // Iterator tags
    using iterator_category = std::forward_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = T;
    using reference         = value_type &;
    using pointer           = value_type *;

    T m_pos, m_posmax; // start/current pos / end of range

    using FuncMove = std::function<int(const int &)>;
    const FuncMove m_fmove;

    T m_tort, m_hare; // Positions of tortoise/hare
    const T m_tsteps = T{1}, m_hsteps = T{2}; // Mover per cycle
    bool m_end;

    CycleIterator(
        const T &start, // steps for hare
        const T &stop, // steps for hare
        const FuncMove &fmove, // function that will move tortoise and hare
        const T &htstart = T{1},  // starting pos for tort/hare
        const T &tsteps = T{1}, // steps for tort
        const T &hsteps = T{2} // steps for hare
    ) : m_pos{start}, m_posmax{stop}, m_fmove{fmove},
        m_tort{htstart}, m_hare{htstart}, m_tsteps{tsteps}, m_hsteps{hsteps},
        m_end(m_pos == m_posmax) {}

    CycleIterator() : m_pos{std::numeric_limits<T>::max()}, m_end{true} {}

    auto operator *() { return true; }
    // auto operator ->() { return &m_val; } // deref to calc

    auto &operator ++() { // Prefix increment
        if (not m_end) {
            for (T i{m_tsteps}; i--; m_tort = m_fmove(m_tort));
            for (T i{m_hsteps}; i--; m_hare = m_fmove(m_hare));
            m_end = (m_tort == m_hare) or (m_pos++ == m_posmax);
        }
        return *this;
    }
    // Postfix increment
    // auto operator ++(int) { CycleIterator tmp = *this; ++(*this); return tmp; }

    auto operator ==(const CycleIterator& o) const {
        return m_end ? o.m_end : (not o.m_end and (m_pos == o.m_pos));
    }
    auto operator !=(const CycleIterator& o) const { return not (*this == o); }
};

template <typename I, typename O>
auto
tortoise_and_hare(I first, I last, O out) {
#ifdef CASE1
    *out++ = std::accumulate(first, last, 0); // +1 til cycle/end
#else // default solution
    *out++ = std::count(first, last, true); // count trues til cycle/end
#endif
}

int
main(int, char *[]) {
    // Prepare iterators for input and output
    auto in = std::istream_iterator<int>{std::cin};
    auto in_last = std::istream_iterator<int>{};
    constexpr auto mod = 1 << 31; // fixed constant
    while(in != in_last) {
        // problem parameters (const and from input)
        auto N = *in++, S = *in++, P = *in++, Q = *in++;
        // Prepare "move" lambda and solve
        auto fmove = [P, Q](const auto &x) { return (x * P + Q) % mod; };
        tortoise_and_hare(
            CycleIterator{1, N, fmove, S % mod},
            CycleIterator{},
            std::ostream_iterator<int>{std::cout}
        );
    }
    return 0;
}
