#include <algorithm> // std::copy
#include <array> // std::array
#include <iostream> // std::cout
#include <iterator> // std::ostream_iterator

template <typename T>
class os_iterator {
    std::ostream_iterator<T> m_iter; // wrapped iterator
    T m_delim; // delimiter between elements
    bool m_dodelim = false; // when to start separating

public:
    // needed for an iterator
    using iterator_category = std::output_iterator_tag;

    // constructor replicating the wrapped iterator's constructor
    os_iterator(std::ostream &os, const T &delim) : m_iter{os}, m_delim{delim} {}

    // no-ops because only the assignment (= operator does something)
    auto operator ->() { return this; }
    auto &operator *() { return *this; }
    auto &operator ++() { return *this; } // ++prefix
    auto &operator ++(int) { return *this; } // postfix++

    // Operation with the wrapped iterator, choosing when to output the sep
    auto &operator =(const T &outval) {
        if (m_dodelim) // output delimiter before the second/later elements
            *m_iter++ = m_delim;
        else
            m_dodelim = true;

        *m_iter++ = outval;
        return *this;
    }
};

template <typename I, typename O>
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
