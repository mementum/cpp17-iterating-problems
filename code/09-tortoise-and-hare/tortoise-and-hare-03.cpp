#include <algorithm> // std::find_if
#include <functional> // std::function
#include <iostream> // std::cin, std::cout
#include <iterator> // std::istream_iterator/ostream_iterator
#include <tuple> // std::get, std::tuple
#include <type_traits> // std::iterator_traits

template <typename T = int>
struct CycleIterator {
    using CycleVal = std::tuple<bool, T>;

    // Iterator tags
    using iterator_category = std::forward_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = CycleVal;
    using reference         = value_type &;
    using pointer           = value_type *;

    T m_pos; // array position

    using FuncMove = std::function<int(const int &)>;
    const FuncMove m_fmove;

    T m_tort, m_hare; // Positions of tortoise/hare
    const T m_tsteps = T{1}, m_hsteps = T{2}; // Mover per cycle

    CycleVal m_val; // cycle detected / array pos

    CycleIterator(
        const T &start, // start position of array length
        const FuncMove &fmove, // function that will move tortoise and hare
        const T &htstart = T{1},  // starting pos for tort/hare
        const T &tsteps = T{1},
        const T &hsteps = T{2} // steps for tort/hare
    ) : m_pos{start - 1}, m_fmove{fmove},m_tort{htstart}, m_hare{htstart},
        m_tsteps{tsteps}, m_hsteps{hsteps} { ++(*this); }

    CycleIterator(const T &stop) : m_pos{stop} {}

    // auto operator ->() { return &m_val; } // deref to calc
    auto &operator *() { return m_val; }

    auto &operator ++() { // Prefix increment
        for (T i{m_tsteps}; i--; m_tort = m_fmove(m_tort));
        for (T i{m_hsteps}; i--; m_hare = m_fmove(m_hare));
        m_val = {m_tort == m_hare, ++m_pos};
        return *this;
    }
    // Postfix increment
    // auto operator ++(int) { CycleIterator tmp = *this; ++(*this); return tmp; }

    auto operator ==(const CycleIterator& o) const { return m_pos == o.m_pos; }
    auto operator !=(const CycleIterator& o) const { return not (*this == o); }
};

template <typename I, typename O>
auto
tortoise_and_hare(I first, I last, O out) {
    // Find if there is cycle (hare meets tortoise) along the range 1=>N
    // and output that point or the length of the range is the result
    auto pred = [](const auto &x) { return std::get<0>(x); };
    *out++ = std::get<1>(*std::find_if(first, last, pred));
}

int
main(int, char *[]) {
    // Prepare iterators for input and output
    auto in = std::istream_iterator<int>{std::cin};
    constexpr auto mod = 1 << 31; // fixed constant
    // problem parameters (const and from input)
    auto N = *in++, S = *in++, P = *in++, Q = *in++;
    // Prepare "move" lambda and solve
    auto fmove = [P, Q](const auto &x) { return (x * P + Q) % mod; };
    tortoise_and_hare(
        CycleIterator{1, fmove, S % mod},
        CycleIterator{N},
        std::ostream_iterator<int>{std::cout}
    );
    return 0;
}
