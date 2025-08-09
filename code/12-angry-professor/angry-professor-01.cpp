#include <algorithm> // std::copy_n
#include <iostream> // std::cout/cin
#include <iterator> // std::istream/ostream_iterator, std::back_inserter
#include <vector> // std::vector

int
main(int, char *[]) {
    auto in = std::istream_iterator<int>{std::cin}; // input iterator
    auto out = std::ostream_iterator<std::string>{std::cout, "\n"}; // out iter
    const auto fearly = [](const auto &x) { return x <= 0; };
    for(auto t = *in++; t--; ++in) { // resync "in" after copy_n operation
        const auto n = *in++, k = *in++; // students and threshold
        auto c = std::vector<int>{}; // storage
        std::copy_n(in, n, std::back_inserter(c)); // copy input
        const auto students = std::count_if(c.begin(), c.end(), fearly);
        *out++ = students < k ? "YES" : "NO"; // canceled if less than k
    }
}
