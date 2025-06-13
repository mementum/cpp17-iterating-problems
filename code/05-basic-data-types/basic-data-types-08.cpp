#include <algorithm> // std::copy_n
#include <iomanip> // std::setprecision
#include <ios> // std::fixed
#include <iostream> // std::cin/cout
#include <iterator> // std::ostream_iterator/istream
#include <type_traits> // std::is_same_v
#include <variant> // std::variant

template <typename Variant, size_t i = 0>
auto
for_each(size_t fprec = 3, size_t dprec = 9) {
    const auto def_prec{std::cout.precision()}; // save current prec
    const auto def_flags{std::cout.flags()}; // save current prec

    using BasicType = std::variant_alternative_t<i, Variant>; // get type

    if constexpr (std::is_same_v<float, BasicType>)
        std::cout << std::fixed << std::setprecision(fprec);
    else if constexpr (std::is_same_v<double, BasicType>)
        std::cout << std::fixed << std::setprecision(dprec);

    auto in = std::istream_iterator<BasicType>{std::cin};
    auto out = std::ostream_iterator<BasicType>{std::cout, "\n"};
    std::copy_n(in, 1, out); // *out++ = *in;

    (std::cout << std::setprecision(def_prec)).flags(def_flags); // reset

    if constexpr ((i + 1) < std::variant_size_v<Variant>)
        for_each<Variant, i + 1>(fprec, dprec);
}

int
main(int, char *[]) {
    using BasicTypes = std::variant<int, long, char, float, double>;
    for_each<BasicTypes>();
    return 0;
}
