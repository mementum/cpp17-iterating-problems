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

using InTypeProcs = std::vector<std::function<std::any(std::istream &)>>;

template <typename Variant, size_t i = 0>
auto
for_each_in(InTypeProcs &intypeprocs) {
    auto intypeproc = [](std::istream &is) {
        using BasicType = std::variant_alternative_t<i, Variant>; // get type
        auto in = std::istream_iterator<BasicType>{is};
        return std::any{*in};
    };
    intypeprocs.push_back(intypeproc);

    if constexpr ((i + 1) < std::variant_size_v<Variant>)
        for_each_in<Variant, i + 1>(intypeprocs);
}

template <typename Variant>
struct vistream_iterator {
    // Iterator tags
    using iterator_category = std::input_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = std::any;
    using reference         = value_type &;
    using pointer           = value_type *;

    InTypeProcs m_intypeprocs;
    size_t m_pos = 0;

    std::istream &m_is;
    bool m_end = false;

    vistream_iterator(std::istream &is)
        : m_is{is}, m_end{not std::variant_size_v<Variant>} {

        for_each_in<Variant>(m_intypeprocs);
    }
    // dummy m_is initialization to avoid using -fpermissive
    vistream_iterator() : m_is{std::cin}, m_end{true} {}

    auto operator ->() const noexcept { return this; }

    auto operator *() const {
        auto intypeproc = *std::next(m_intypeprocs.begin(), m_pos);
        return intypeproc(m_is);
    }
    // Prefix increment
    auto operator ++() {
        m_end = (++m_pos == std::variant_size_v<Variant>);
        return *this;
    }
    // Postfix increment
    auto operator ++(int) const {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    auto operator ==(const vistream_iterator& other) const {
        return m_end ? other.m_end : (other.m_end ? false : m_pos == other.m_pos);
    };
    auto operator !=(const vistream_iterator& other) const {
        return not (*this == other);
    }
};

///////////////////////////////////////////////////////////////////////////
using OutTypeProcs = std::vector<
    std::function<void(std::ostream &, const std::any &, size_t, size_t)>>;

template <typename Variant, size_t i = 0>
auto
for_each_out(OutTypeProcs &otypeprocs) {
    auto proc = [](std::ostream &os, const std::any &val,
                   size_t fprec = 3, size_t dprec = 9) {

        using BasicType = std::variant_alternative_t<i, Variant>; // get type
        const auto def_prec{os.precision()}; // save current prec
        const auto def_flags{os.flags()}; // save current prec
        auto out = std::ostream_iterator<BasicType>{os};
        if constexpr (std::is_same_v<float, BasicType>)
            os << std::fixed << std::setprecision(fprec);
        else if constexpr (std::is_same_v<double, BasicType>)
            os << std::fixed << std::setprecision(dprec);

        *out++ = std::any_cast<const BasicType &>(val);
        (os << std::setprecision(def_prec)).flags(def_flags); // reset
    };
    otypeprocs.push_back(proc);

    if constexpr ((i + 1) < std::variant_size_v<Variant>)
        for_each_out<Variant, i + 1>(otypeprocs);
}

template <typename Variant>
class vostream_iterator {
public:
    // needed for an iterator
    using iterator_category = std::output_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = std::any;
    using reference         = value_type &;
    using pointer           = value_type *;

private:
    OutTypeProcs m_otypeprocs;
    size_t m_pos = 0;

    std::ostream &m_os;
    std::string m_delim;
    size_t m_fprec;
    size_t m_dprec;

public:
    // constructor replicating the wrapped iterator's constructor
    vostream_iterator(std::ostream &os, const std::string &delim,
        size_t fprec = 3, size_t dprec = 9)
        : m_os{os}, m_delim(delim), m_fprec(fprec), m_dprec(dprec) {

        for_each_out<Variant>(m_otypeprocs);
    }
    // no-ops because only the assignment (= operator does something)
    auto operator ->() const { return this; };
    auto &operator *() const { return *this; }
    auto &operator ++() { ++m_pos; return *this; } // ++prefix
    auto operator ++(int) const { //postfix++
        auto tmp = *this;
        ++(*this);
        return tmp;
    }
    // Operation with the wrapped iterator, choosing when to output the sep
    auto &operator =(const std::any &val) const {
        auto otypeproc = *std::next(m_otypeprocs.begin(), m_pos);
        otypeproc(m_os, val, m_fprec, m_dprec);
        m_os << m_delim;
        return *this;
    }
};

// Solution
template<typename I, typename O>
auto
basic_types(I first, I last, O out) {
    return std::copy(first, last, out);
}

int
main(int, char *[]) {
    using BasicTypes = std::variant<int, long, char, float, double>;
    basic_types(
        vistream_iterator<BasicTypes>{std::cin},
        vistream_iterator<BasicTypes>{},
        vostream_iterator<BasicTypes>{std::cout, "\n"}
    );
    return 0;
}
