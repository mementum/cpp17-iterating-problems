#include <algorithm> // std::copy_n
#include <iomanip> // std::setprecision
#include <ios> // std::fixed
#include <iostream> // std::cin/cout
#include <iterator> // std::ostream_iterator/istream
#include <utility> // std::make_pair

template <typename BasicType>
auto
in_out() {
    auto in = std::istream_iterator<BasicType>{std::cin};
    auto out = std::ostream_iterator<BasicType>{std::cout, "\n"};
    return std::make_pair(in, out);
}

template<typename BasicType>
auto
for_each(size_t /* fprec */ = 3, size_t /* dprec */ = 9) {
    auto [in, out] = in_out<BasicType>();
    std::copy_n(in, 1, out); // *out++ = *in;
}

template<typename BasicType>
auto
for_each_floating(size_t prec) {
    const auto def_prec{std::cout.precision()}; // save current prec
    const auto def_flags{std::cout.flags()}; // save current prec
    std::cout << std::fixed << std::setprecision(prec);
    auto [in, out] = in_out<BasicType>();
    std::copy_n(in, 1, out); // *out++ = *in;
    (std::cout << std::setprecision(def_prec)).flags(def_flags); // reset
}

template<>
auto
for_each<float>(size_t fprec /* = 3 */, size_t /* dprec = 9 */) {
    for_each_floating<float>(fprec);
}

template<>
auto
for_each<double>(size_t /* fprec = 3 */, size_t dprec /* = 9 */) {
    for_each_floating<double>(dprec);
}

template<typename BasicType, typename BasicType2, typename ...BasicTypes>
auto
for_each(size_t fprec = 3, size_t dprec = 9) {
    for_each<BasicType>(fprec, dprec);
    for_each<BasicType2, BasicTypes...>(fprec, dprec);
}

int
main(int, char *[]) {
    for_each<int, long, char, float, double>();
    return 0;
}
