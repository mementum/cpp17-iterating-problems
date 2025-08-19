#include <iostream> // std::cout/cin
#include <iomanip> // std::setprecision, ...
#include <iterator> // std::istream/ostream_iterator, std::back_inserter
#include <set> // std::set

// Main
int
main(int, char *[]) {
    auto in = std::istream_iterator<int>{std::cin}; // input iterator
    auto out = std::ostream_iterator<double>{std::cout, "\n"}; // out iter
    std::cout << std::fixed << std::setprecision(1);
    auto s = std::set<int>{};
    for(auto t = *in++; t--;) { // number of elements until exhausted
        s.insert(*in++);
        auto left = static_cast<int>((s.size() - 1) / 2);
        double val = static_cast<double>(*std::next(s.begin(), left));
        if (not (s.size() % 2))
            val = (val + *std::next(s.begin(), left + 1)) / 2;
        *out++ = val;
    }
}
