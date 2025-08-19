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
    auto v = std::vector<int>{}; // allow repeated keys
    auto t = *in++; // get number of incoming int values
    v.reserve(t); // reserve the capacity needed
    for(auto odd = 1, val = 0; t--; odd = not odd) {
        v.push_back(val = *in++); // fetch next val
        std::sort(v.begin(), v.end());
        auto left = static_cast<int>((v.size() - 1) / 2);
        auto outval = static_cast<double>(v[left]);
        if (not odd) // median is avg of 2 values if size is even
            outval = (outval + static_cast<double>(v[left + 1])) / 2;
        *out++ = outval;
    }
}
