#include <algorithm> // std::cout/cin
#include <iostream> // std::cout/cin
#include <iomanip> // std::setprecision, ...
#include <iterator> // std::istream/ostream_iterator, std::back_inserter
#include <vector> // std::vector

// Main
int
main(int, char *[]) {
    auto in = std::istream_iterator<int>{std::cin}; // input iterator
    auto out = std::ostream_iterator<double>{std::cout, "\n"}; // out iter
    std::cout << std::fixed << std::setprecision(1); // fixed 1 decimal
    auto v = std::vector<int>{};
    for(auto t = *in++, odd = 1; t--; odd = not odd) {
        auto val = *in++; // fetch next val
#ifdef CASE1
        const auto fgreater = [&val](const auto &x) { return val > x; };
        auto idx = std::find_if(v.begin(), v.end(), fgreater);
#else
        constexpr auto fgreater = std::greater<int>{};
        auto idx = std::lower_bound(v.begin(), v.end(), val, fgreater);
#endif
        v.insert(idx, val); // insert val just before idx
        auto left = static_cast<int>((v.size() - 1) / 2);
        auto outval = static_cast<double>(v[left]);
        if (not odd) // median is avg of 2 values if size is even
            outval = (outval + static_cast<double>(v[left + 1])) / 2;
        *out++ = outval;
    }
    return 0;
}
