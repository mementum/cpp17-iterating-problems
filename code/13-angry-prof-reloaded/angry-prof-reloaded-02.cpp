#include <array> // std::array
#include <algorithm> // std::copy_n, std::partition
#include <iostream> // std::cout/cin
#include <iterator> // std::istream/ostream_iterator
#include <vector> // std::vector

template <typename C, typename F>
struct bool_back_insert_iterator {
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
    bool_back_insert_iterator(C &c, const F &f) : m_c{c}, m_f{f} {}

    // no-ops because only the assignment (= operator does something)
    auto *operator ->() const { return this; } // we wouldn't need this
    auto &operator *() const { return *this; }
    auto &operator ++() const { return *this; } // ++prefix
    auto &operator ++(int) const { return *this; } // postfix++

    using ContValType = typename C::value_type;

    // Only Operational Method
    auto &operator =(const ContValType &value) const {
        if (m_f(value))
            m_c.push_back(value);
        return *this;
    }
};

template<typename C, typename F>
auto
bool_back_inserter(C &c, const F &fearly) {
    return bool_back_insert_iterator<C, F>(c, fearly);
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
        std::copy_n(in, n, bool_back_inserter(c, fearly)); // copy input
        *out++ = canceled[c.size() < static_cast<unsigned int>(k)];
    }
    return 0;
}
