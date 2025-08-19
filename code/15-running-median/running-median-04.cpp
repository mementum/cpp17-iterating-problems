#include <algorithm> // std::cout/cin
#include <iostream> // std::cout/cin
#include <iomanip> // std::setprecision, ...
#include <iterator> // std::istream/ostream_iterator, std::back_inserter
#include <list> // std::list

// Main
int
main(int, char *[]) {
    auto in = std::istream_iterator<int>{std::cin}; // input iterator
    auto out = std::ostream_iterator<double>{std::cout, "\n"}; // out iter
    std::cout << std::fixed << std::setprecision(1); // fixed 1 decimal
    auto l = std::list<int>{}; // allow repeated keys
    auto rmed = l.begin(); // running median iterator, ref to end
    auto inspos = rmed;
    for(auto t = *in++, odd = 1, rval = 0; t--; odd = not odd) {
        auto val = *in++; // fetch next val
        if (val >= rval) {
            auto fpos = [val](auto x) { return val <= x; }; // to find insert pos
            inspos = std::find_if(std::next(rmed), l.end(), fpos); // find the pos
        } else {
            auto fpos = [val](auto x) { return val < x; }; // to find insert pos
            inspos = std::find_if(l.begin(), rmed, fpos); // find the pos
        }
        l.insert(inspos, val); // insert val just before inspos
        rmed = (val >= rval) ? std::next(rmed, odd) : std::prev(rmed, not odd);
        auto outval = static_cast<double>(rval = *rmed);
        if (not odd) // median is avg of 2 values if size is even
            outval = (outval + static_cast<double>(*std::next(rmed))) / 2;
        *out++ = outval;
    }
}
