#include <array> // std::array
#include <algorithm> // std::copy_n, std::partition
#include <iostream> // std::cout/cin
#include <iterator> // std::istream/ostream_iterator, std::back_inserter
#include <vector> // std::vector

// custom count algorithm
template <typename C, typename F>
auto
count_early_students(C &cont, const F &fnk) {
    const auto pivot = std::partition(cont.begin(), cont.end(), fnk);
    return std::distance(cont.begin(), pivot);
}

// Main
int
main(int, char *[]) {
    auto in = std::istream_iterator<int>{std::cin}; // input iterator
    auto in_last = std::istream_iterator<int>{}; // input iterator end
    auto out = std::ostream_iterator<std::string>{std::cout, "\n"}; // out iter
    constexpr auto canceled = std::array{"NO", "YES"};
    const auto fearly = [](const auto &x) { return x <= 0; };
    [[maybe_unused]] auto t = *in++; // number of testcases (unused)
    for(; in != in_last; ++in) { // resync "in" after copy_n operation
        const auto n = *in++, k = *in++; // students and threshold
        auto c = std::vector<int>{}; // storage
        std::copy_n(in, n, std::back_inserter(c)); // copy input
        const auto students = count_early_students(c, fearly);
        *out++ = canceled[students < k];
    }
}
