#include <array> // std::array
#include <algorithm> // std::copy_n
#include <functional> // std::function
#include <iostream> // std::cout/cin
#include <iterator> // std::istream/ostream_iterator, std::back_inserter
#include <vector> // std::vector

template <typename C>
struct CountUntilIter {
    // Iterator tags
    using iterator_category = std::forward_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = typename C::value_type;
    using reference         = value_type &;
    using pointer           = value_type *;

    typename C::const_iterator m_first;
    typename C::const_iterator m_last;
    int m_until;
    int m_count = 0;

    using FuncEarly = std::function<bool(int)>;
    FuncEarly m_fearly;

    bool m_end = true;

    CountUntilIter(const C &c, int until, FuncEarly fearly)
        : m_first{c.cbegin()}, m_last{c.cend()}, m_until{until},
          m_fearly{fearly}, m_end{m_count >= m_until or m_first == m_last} {};

    CountUntilIter() {}

    auto operator *() {
        auto ret = m_fearly(*m_first);
        if (not m_end and ret)
            m_end = m_until == (m_count += ret);
        return ret;
    }
    auto &operator ++() { // Prefix increment
        if (not m_end)
            m_end = ++m_first == m_last;
        return *this;
    }
    auto operator ==(const CountUntilIter& o) const {
        return o.m_end ? m_end : m_first == o.m_first;
    }
    auto operator !=(const CountUntilIter& o) const { return not (*this == o); }
};

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
        const auto students = std::count(
            CountUntilIter{c, k, fearly},
            CountUntilIter<decltype(c)>{},
            true
        );
        *out++ = canceled[students < k];
    }
    return 0;
}
