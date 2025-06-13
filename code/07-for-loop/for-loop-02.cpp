#include <algorithm> // std::transform
#include <array> // std::array
#include <iostream> // std::cin/cout
#include <iterator> // std::istream/ostream_iterator
#include <limits> // std::numeric_limits

template <typename T=int>
class Range {
    struct StartStopStep {
        const T start = 0;
        const T stop = std::numeric_limits<T>::max();
        const T step = 1;
    } m_sss;

public:
    Range(const StartStopStep& sss) : m_sss{sss} {};
    Range(const T stop) : m_sss{.stop=stop} {};
    Range(const T start, const T stop, const T step = 1)
        : m_sss{.start=start, .stop=stop, .step=step} {};

private:
    struct Iter {
    private:
        StartStopStep m_sss;
        T m_pos;

    public:
        // Iterator tags
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = T;
        using reference         = value_type; // usually value_type &
        using pointer           = value_type; // usually value_type *

        Iter(const StartStopStep &sss) : m_sss{sss}, m_pos{m_sss.start} {};
        Iter(const T &pos) : m_sss{.stop=pos}, m_pos{pos}  {}; // en-of-range

        auto operator *() const { return m_pos; }
        auto operator ->() const { return m_pos; }
        auto& operator ++() { // Prefix increment - increase until stop
            m_pos = std::min(m_pos + m_sss.step, m_sss.stop);
            return *this;
        }
        // Postfix increment
        auto operator ++(int) { Iter tmp = *this; ++(*this); return tmp; }

        auto operator ==(const Iter& o) const { return m_pos == o.m_pos; }
        auto operator !=(const Iter& o) const { return m_pos != o.m_pos; }
    };

public:
    auto begin() const { return Iter{m_sss}; } // copy range, pos at start
    auto end() const { return Iter{m_sss.stop}; } // place directly at end
};

constexpr auto numbers = std::array{ // we do not need the zero index, a >= 1
    "even", // index 0
    "one", "two", "three", "four", "five", "six", "seven", "eight", "nine",
    "odd" // index 10
};

template <typename I, typename O>
auto
num2words(I first, I last, O out) {
    auto n2w = [](auto i) { return numbers[i < 10 ? i : 10 * (i % 2)]; };
    std::transform(first, last, out, n2w);
}

int
main(int, char *[]) {
    auto in = std::istream_iterator<int>{std::cin}; // prepare input iterator
    auto a = *in++, b = *in++; // gather parameters
    auto range = Range{a, b + 1};  // our range is half-open, need extra +1
    num2words(
        range.begin(), range.end(),
        std::ostream_iterator<std::string>{std::cout, "\n"}
    );

    return 0;
}
