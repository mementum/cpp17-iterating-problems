#include <any> // std::any
#include <algorithm> // std::copy_n
#include <functional> // std::function
#include <iomanip> // std::setprecision
#include <ios> // std::fixed
#include <iostream> // std::cin/cout
#include <iterator> // std::ostream_iterator/istream
#include <map> // std::map
#include <type_traits> // std::is_same_v
#include <variant> // std::variant
#include <vector> // std::vector
#include <typeindex> // std::type_index

using BasicTypes = std::variant<int, long, char, float, double>;

using InTypeProc = std::function<std::any(std::istream &)>;
using InTypeProcs = std::vector<InTypeProc>;

using OutTypeProc = std::function<void(std::ostream &, const std::any &)>;
using OutTypeProcs = std::map<std::type_index, OutTypeProc>;

static InTypeProcs intypeprocs; // storage for the input fetching procs
static OutTypeProcs outtypeprocs;  // storage for the output procs

template <typename Variant, size_t i = 0>
auto
for_each(InTypeProcs &itp, OutTypeProcs &otp) {
    using BasicType = std::variant_alternative_t<i, Variant>; // get type
    auto intypeproc = [](std::istream &is) {
        return std::any{*std::istream_iterator<BasicType>{is}};
    };
    itp.push_back(intypeproc);

    auto outtypeproc = [](std::ostream &os, const std::any &a) {
        os << std::any_cast<BasicType>(a);
    };
    auto typeidx = std::type_index(typeid(BasicType));
    otp.insert(std::make_pair(typeidx, outtypeproc));

    if constexpr ((i + 1) < std::variant_size_v<Variant>)
        for_each<Variant, i + 1>(itp, otp);
}

template <typename Variant>
struct vistream_iterator {
    // Iterator tags
    using iterator_category = std::input_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = std::any;
    using reference         = value_type &;
    using pointer           = value_type *;

    size_t m_pos = 0;

    std::istream &m_is = std::cin;
    bool m_end = true;

    vistream_iterator(std::istream &is) :
        m_is{is}, m_end{not std::variant_size_v<Variant>} {}
    vistream_iterator() {}

    auto operator ->() const noexcept { return this; }
    auto operator *() const {
        auto intypeproc = *std::next(intypeprocs.begin(), m_pos);
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
    auto operator ++(int) const { return *this; }

    auto &operator =(const std::any &a) const {
        const auto def_prec{m_os.precision()}; // save current prec
        const auto def_flags{m_os.flags()}; // save current flags

        if (a.type() == typeid(float))
            m_os << std::fixed << std::setprecision(m_fprec);
        else if (a.type() == typeid(double))
            m_os << std::fixed << std::setprecision(m_dprec);

        // use structured bindings to get our out-type-proc
        auto &[_, otproc] = *outtypeprocs.find(std::type_index(a.type()));
        otproc(m_os, a);

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
    for_each<BasicTypes>(intypeprocs, outtypeprocs);
    basic_types(
        vistream_iterator<BasicTypes>{std::cin},
        vistream_iterator<BasicTypes>{},
        vostream_iterator{std::cout, "\n"}
    );
    return 0;
}
