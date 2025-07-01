#include <algorithm> // std::transform
#include <iostream> // std::cin/cout
#include <iterator> // std::istream/ostream_iterator
#include <limits> // std::numeric_limits
#include <map> // std::map
#include <stdexcept> // std::out_of_range
#include <type_traits> // std::enable_if, std::is_integral, std::void_t

using PTYPE = int;  // problem type for several definitions

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

template <typename I, typename F, typename C>
using i2f_type = std::invoke_result_t<F, it_type<I>, C>;

template<typename, typename, typename, typename, typename = void>
constexpr bool i2f2o_v = false;

template<typename I, typename O, typename F, typename C>
constexpr bool i2f2o_v<I, O, F, C,
    std::void_t<decltype(std::declval<O>() = std::declval<i2f_type<I, F, C>>())>
    > = true;

template <typename C, typename = void>
constexpr bool has_method_at = false;

template <typename C>
constexpr bool has_method_at<C,
    std::void_t<decltype(std::declval<C>().at(PTYPE{}))>> = true;

template <typename I, typename O, typename F, typename C>
using enable_if_iof = std::enable_if_t<
    io_iterators_v<I, O> && i2f2o_v<I, O, F, C> && has_method_at<C>>;

// SFINAE is over
auto numbers = std::unordered_map<PTYPE, std::string>{
    {6, "six"}, {7, "seven"}, {8, "eight"}, {9, "nine"},
    {1, "one"}, {2, "two"}, {3, "three"}, {4, "four"}, {5, "five"},
};

template <typename I, typename O, typename F, typename C,
    typename = enable_if_iof<I, O, F, C>>
auto
num2words(I first, I last, O out, F n2w, const C &container) {
    auto _n2w = std::bind(n2w, std::placeholders::_1, container);
    std::transform(first, last, out, _n2w);
}

int
main(int, char *[]) {
    auto in = std::istream_iterator<PTYPE>{std::cin}; // prepare input iterator
    auto a = *in++, b = *in++; // gather parameters
    auto range = Range{a, b + 1};  // our range is half-open, need extra +1

    using namespace std::string_literals;

    auto n2w = [](auto i, const auto &container) {
        try {
            return container.at(i);
        } catch(const std::out_of_range & /* e */) {
            return (i % 2) ? "odd"s : "even"s;
        }
    };

    num2words(
        range.begin(), range.end(),
        std::ostream_iterator<std::string>{std::cout, "\n"},
        n2w,
        numbers
    );

    return 0;
}
