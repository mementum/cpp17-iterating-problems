#include <algorithm> // std::find_if
#include <functional> // std::function
#include <iostream> // std::cin, std::cout
#include <iterator> // std::istream_iterator/ostream_iterator
#include <limits> // std::numeric_limits
#include <type_traits> // std::enable_if, std::is_integral, std::void_t

template <typename T>
using enable_if_integral = std::enable_if_t<std::is_integral_v<T>>;

template <typename T = int, typename = enable_if_integral<T>>
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
        Iter(const T &pos) : m_sss{.stop=pos}, m_pos{pos}  {}; // end-of-range

        auto operator *() const { return m_pos; }
        auto operator ->() const { return &m_pos; }
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

template <typename T>
using it_type = typename std::iterator_traits<T>::value_type;

template <typename T>
using Predicate = std::function<bool(const it_type<T> &)>;

template <typename I, typename O>
auto
tortoise_and_hare(I first, I last, O out, const Predicate<I> &pred) {
    // Find cycle (hare meets tortoise) along the range 1=>N or end of range
    *out++ = *std::find_if(first, last, pred);
}

int
main(int, char *[]) {
    // Prepare iterators for input and output
    auto in = std::istream_iterator<int>{std::cin};
    auto out = std::ostream_iterator<int>{std::cout};
    // problem parameters (const and from input)
    constexpr int mod = 1 << 31;
    auto N = *in++, S = *in++, P = *in++, Q = *in++;
    // Initialize the tortoise & hare positions, and prepare calculating lambdas
    auto tort = S % mod, hare = tort;
    auto fmove = [P, Q](const auto &x) { return (x * P + Q) % mod; };
    auto ftort = [&fmove, &tort]() { return tort = fmove(tort); };
    auto fhare = [&fmove, &hare]() { return hare = fmove(fmove(hare)); };
    auto ftoha = [&ftort, &fhare](const auto &) { return ftort() == fhare(); };

    auto range = Range{1, N}; // Prepare a lazy evaluation Range from 1 to N
    tortoise_and_hare(range.begin(), range.end(), out, ftoha); // solve
    return 0;
}
