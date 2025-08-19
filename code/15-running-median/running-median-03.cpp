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
    auto rmed = s.end(); // running median iterator
    for(auto t = *in++, odd = 1, val = 0, rval = 0; t--; odd = not odd) {
        s.insert(val = *in++); // add to multi-set and store in "val"
        rmed = (val >= rval) ? std::next(rmed, odd) : std::prev(rmed, not odd);
        auto outval = static_cast<double>(rval = *rmed); // rval for next round
        if (not odd) // median is avg of 2 values if size is even
            outval = (outval + static_cast<double>(*std::next(rmed))) / 2;
        *out++ = outval;
    }
}
