#include <algorithm> // std::copy_n
#include <iomanip> // std::setprecision
#include <ios> // std::fixed
#include <iostream> // std::cin/cout
#include <iterator> // std::ostream_iterator/istream
#include <type_traits> // std::is_same_v

template<typename ...BasicTypes>
auto
for_each(size_t fprec = 3, size_t dprec = 9) {
    auto in_out = [&fprec, &dprec](auto x) {
        const auto def_prec{std::cout.precision()}; // save current prec
        const auto def_flags{std::cout.flags()}; // save current flags

        using BasicType = decltype(x);  // deduct type of dummy x value
        if constexpr (std::is_same_v<float, BasicType>)
            std::cout << std::fixed << std::setprecision(fprec);
        else if constexpr (std::is_same_v<double, BasicType>)
            std::cout << std::fixed << std::setprecision(dprec);

        auto in = std::istream_iterator<BasicType>{std::cin};
        auto out = std::ostream_iterator<BasicType>{std::cout, "\n"};
        std::copy_n(in, 1, out); // *out++ = *in;

        (std::cout << std::setprecision(def_prec)).flags(def_flags); // reset
    };

    (in_out(BasicTypes{}), ...);
}

int
main(int, char *[]) {
    for_each<int, long, char, float, double>();
    return 0;
}
