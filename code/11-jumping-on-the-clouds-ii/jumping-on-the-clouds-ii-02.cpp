#include <algorithm> // std::copy_n
#include <functional> // std::function
#include <iostream> // std::cout/cin
#include <iterator> // std::istream/ostream_iterator
#include <numeric> // std::accumulate
#include <vector> // std::vector

template <typename It, typename T = int>
struct JumpingIterator {
    // Iterator tags
    using iterator_category = std::random_access_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = T;
    using reference         = value_type &;
    using pointer           = value_type *;

    const It m_itfirst;
    It m_itcur;
    using FunctionMove = std::function<int(int)>;
    FunctionMove m_fmove;

    bool m_end = false;

    JumpingIterator(It first, const FunctionMove &fmove)
        : m_itfirst{first}, m_itcur{first}, m_fmove(fmove) {}

    JumpingIterator() : m_end{true} {}

    auto operator ->() { return &(*m_itcur); }
    auto operator *() { return *m_itcur; }

    auto &operator ++() { // Prefix increment
        if (not m_end) {
            auto d = std::distance(m_itfirst, m_itcur); // >= 0
            m_itcur = std::next(m_itfirst, m_fmove(d));
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

template <typename I, typename FMove, typename FEnergy>
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
    return 0;
}
