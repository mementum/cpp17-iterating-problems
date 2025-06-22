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

template <typename Variant, size_t i = 0>
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

template <typename I>
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

    auto operator ==(const vistream_iterator& other) const {
        return m_end ? other.m_end : (other.m_end ? false : m_first == other.m_first);
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

    template<typename V>
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

// Solution
template<typename I, typename O>
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
