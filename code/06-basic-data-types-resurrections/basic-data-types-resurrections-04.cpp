#include <any> // std::any
#include <algorithm> // std::copy_n
#include <functional> // std::function
#include <iomanip> // std::setprecision
#include <ios> // std::fixed
#include <iostream> // std::cin/cout
#include <iterator> // std::ostream_iterator/istream
#include <type_traits> // std::is_same_v
#include <variant> // std::variant
#include <vector> // std::vector

using BasicTypes = std::variant<int, long, char, float, double>;
using InTypeProc = std::function<BasicTypes(std::istream &)>;
using InTypeProcs = std::vector<InTypeProc>;

static InTypeProcs intypeprocs;

// SFINAE to check for a variant
template<typename V>
struct is_variant_t : std::false_type {};

// Need to specialize to consider a template pack, for std::variant
template<typename ...Args>
struct is_variant_t<std::variant<Args...>> : std::true_type {};

template<typename V>
constexpr bool is_variant_v = is_variant_t<V>::value;

template<typename V>
using enable_if_variant = std::enable_if_t<is_variant_v<V>>*;

template <typename Variant, size_t i = 0, enable_if_variant<Variant> = nullptr>
auto
for_each_in(InTypeProcs &itp) {
    using BasicType = std::variant_alternative_t<i, Variant>; // get type
    auto intypeproc = [](std::istream &is) {
        return Variant{*std::istream_iterator<BasicType>{is}};
    };
    itp.push_back(intypeproc);

    if constexpr ((i + 1) < std::variant_size_v<Variant>)
        for_each_in<Variant, i + 1>(itp);
}

// SFINAE to check for I being an Input iterator and delivering a variant
template <typename T, typename Tag>
constexpr bool is_it_tag_v = std::is_base_of_v<
    Tag, typename std::iterator_traits<T>::iterator_category>;

template <typename I>
constexpr bool is_input_v = is_it_tag_v<I, std::input_iterator_tag>;

template <typename I>
constexpr bool input_variant_v = is_variant_v<
    typename std::decay_t<decltype(*std::declval<I>())>::result_type>;

template <typename I>
constexpr bool is_input_variant_v = is_input_v<I> && input_variant_v<I>;

template<typename I>
using enable_if_input_variant = std::enable_if_t<is_input_variant_v<I>>*;

template <typename I, enable_if_input_variant<I> = nullptr>
struct vistream_iterator {
    // Iterator tags
    using iterator_category = std::input_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = std::any;
    using reference         = value_type &;
    using pointer           = value_type *;

    std::istream &m_is = std::cin;
    I m_first, m_last;
    bool m_end = true;

    vistream_iterator(std::istream &is, I first, I last)
        : m_is{is}, m_first{first}, m_last{last}, m_end{first == last} {}
    vistream_iterator(I last) : m_first{last}, m_last{last} {}

    auto operator ->() const noexcept { return this; }
    auto operator *() const {
        return (*m_first)(m_is); // fetched intypeproc delivers result
    }
    // Prefix increment
    auto &operator ++() {
        m_end = ((++m_first) == m_last);
        return *this;
    }
    // Postfix increment
    auto operator ++(int) const {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    auto operator ==(const vistream_iterator& o) const {
        return m_end ? o.m_end : (o.m_end ? false : m_first == o.m_first);
    };
    auto operator !=(const vistream_iterator& other) const {
        return not (*this == other);
    }
};

///////////////////////////////////////////////////////////////////////////
class vostream_iterator {
public:
    // needed for an iterator
    using iterator_category = std::output_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = std::any;
    using reference         = value_type &;
    using pointer           = value_type *;

private:
    std::ostream &m_os;
    std::string m_delim;
    size_t m_fprec;
    size_t m_dprec;

public:
    // constructor replicating the wrapped iterator's constructor
    vostream_iterator(std::ostream &os, const std::string &delim,
        size_t fprec = 3, size_t dprec = 9)
        : m_os{os}, m_delim{delim}, m_fprec{fprec}, m_dprec{dprec} {}

    // no-ops because only the assignment (= operator does something)
    auto operator ->() const { return this; };
    auto &operator *() const { return *this; }
    auto &operator ++() { return *this; } // ++prefix
    auto &operator ++(int) { return *this; } // postfix++

    template<typename V, enable_if_variant<V> = nullptr>
    auto &operator =(const V &v) const {
        const auto def_prec{m_os.precision()}; // save current prec
        const auto def_flags{m_os.flags()}; // save current flags

        // visit our variant val and get the type in auto format
        std::visit([&](auto &arg) {  // arg is the value inside variant v
            using T = std::decay_t<decltype(arg)>; // deduce actual type
            // do something if float or double
            if constexpr (std::is_same_v<T, float>)
                m_os << std::fixed << std::setprecision(m_fprec);
            else if constexpr (std::is_same_v<T, double>)
                m_os << std::fixed << std::setprecision(m_dprec);

            m_os << arg; // m_os can already work with arg
        }, v);

        (m_os << std::setprecision(def_prec)).flags(def_flags); // reset
        m_os << m_delim;
        return *this;
    }
};

// SFINAE (extra, input is taken from above) for the general solution
template <typename O>
constexpr bool is_output_v = is_it_tag_v<O, std::output_iterator_tag>;

template<typename I, typename O>
constexpr bool io_iterators_v = is_input_v<I> && is_output_v<O>;

template <typename I, typename O>
using io_type = decltype(*std::declval<O>() = *std::declval<I>());

template<typename, typename, typename = void>
struct io_i2o : std::false_type {};

template<typename I, typename O>
struct io_i2o<I, O, std::void_t<io_type<I, O>>>
    : std::true_type {};

template<typename I, typename O>
constexpr bool io_i2o_v = io_i2o<I, O>::value;

template<typename I, typename O>
using enable_if_io = std::enable_if_t<io_iterators_v<I, O> && io_i2o_v<I, O>>*;

// Solution
template<typename I, typename O, enable_if_io<I, O> = nullptr>
auto
basic_types(I first, I last, O out) {
    return std::copy(first, last, out);
}

int
main(int, char *[]) {
    for_each_in<BasicTypes>(intypeprocs);
    basic_types(
        vistream_iterator{std::cin, intypeprocs.begin(), intypeprocs.end()},
        vistream_iterator{intypeprocs.end()},
        vostream_iterator{std::cout, "\n"}
    );
    return 0;
}
