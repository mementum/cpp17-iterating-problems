#include <iostream> // std::cout/cin
#include <iomanip> // std::setprecision, ...
#include <iterator> // std::istream/ostream_iterator, std::back_inserter
#include <set> // std::multiset

// Main
int
main(int, char *[]) {
    auto in = std::istream_iterator<int>{std::cin}; // input iterator
    auto out = std::ostream_iterator<double>{std::cout, "\n"}; // out iter
    std::cout << std::fixed << std::setprecision(1); // fixed 1 decimal
    auto s = std::multiset<int>{}; // allow repeated keys
    for(auto t = *in++; t--;) { // number of elements until exhausted
        s.insert(*in++); // add to multi-set
        auto left = static_cast<int>((s.size() - 1) / 2); // pos of left val
        auto itleft = std::next(s.begin(), left); // iterator to left median val
        auto outval = static_cast<double>(*itleft);
        if (not (s.size() % 2)) // median is avg of 2 values if size is even
            outval = (outval + *(++itleft)) / 2;
        *out++ = outval;
    }
}
