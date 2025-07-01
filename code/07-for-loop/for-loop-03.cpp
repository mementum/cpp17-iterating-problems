#include <algorithm> // std::transform
#include <array> // std::array
#include <iostream> // std::cin/cout
#include <iterator> // std::istream/ostream_iterator
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

// SFINAE to check for I being an Input iterator and delivering a variant
template <typename T, typename Tag>
constexpr bool is_it_tag_v =
    std::is_base_of_v<Tag, typename std::iterator_traits<T>::iterator_category>;

template <typename I>
constexpr bool is_input_v = is_it_tag_v<I, std::input_iterator_tag>;

template <typename O>
constexpr bool is_output_v = is_it_tag_v<O, std::output_iterator_tag>;

template<typename I, typename O>
constexpr bool io_iterators_v = is_input_v<I> && is_output_v<O>;

template <typename T>
using it_type = typename std::iterator_traits<T>::value_type;

template <typename I, typename F>
using i2f_type = std::invoke_result_t<F, it_type<I>>;

template<typename, typename, typename, typename = void>
constexpr bool i2f2o_v = false;

template<typename I, typename O, typename F>
constexpr bool i2f2o_v<I, O, F,
    std::void_t<decltype(std::declval<O>() = std::declval<i2f_type<I, F>>())>
    >
    = true;

template <typename I, typename O, typename F>
using enable_if_iof = std::enable_if_t<io_iterators_v<I, O> && i2f2o_v<I, O, F>>;

template <typename I, typename O, typename F, typename = enable_if_iof<I, O, F>>
auto
num2words(I first, I last, O out, F n2w) {
    std::transform(first, last, out, n2w);
}

// SFINAE is over
constexpr auto numbers = std::array{ // we do not need the zero index, a >= 1
    "even", // index 0
    "one", "two", "three", "four", "five", "six", "seven", "eight", "nine",
    "odd" // index 10
};

int
main(int, char *[]) {
    auto in = std::istream_iterator<int>{std::cin}; // prepare input iterator
    auto a = *in++, b = *in++; // gather parameters
    auto range = Range{a, b + 1};  // our range is half-open, need extra +1

#ifdef CASE1
    // SFINAE => NOK - 1 parameters but returns int
    auto n2w = [](auto i) { return i; };
#elif defined CASE2
    // SFINAE => OK - 1 parameter and returns string - solution FAILS
    auto n2w = [](auto i) { return std::string{}; };
#elif defined CASE3
    // SFINAE => NOK - 2 parameters instead of 1
    auto n2w = [](auto i, auto x) { return std::string{}; };
#else // Default - The problem solution
    // SFINAE => OK - 1 param, returns convertible to std::string
    auto n2w = [](auto i) { return numbers[i < 10 ? i : 10 * (i % 2)]; };
#endif

    num2words(
        range.begin(), range.end(),
        std::ostream_iterator<std::string>{std::cout, "\n"},
        n2w
    );

    return 0;
}
