#include <functional> // std::greater
#include <iostream> // std::cout/cin
#include <iomanip> // std::setprecision, ...
#include <iterator> // std::istream/ostream_iterator, std::back_inserter
#include <queue> // std::priority_queue
#include <vector> // std::vector

// Main
int
main(int, char *[]) {
    auto in = std::istream_iterator<int>{std::cin}; // input iterator
    auto out = std::ostream_iterator<double>{std::cout, "\n"}; // out iter
    std::cout << std::fixed << std::setprecision(1); // fixed 1 decimal
    auto ql = std::priority_queue<int>{}; // left heap / right heap (inverted)
    auto qr = std::priority_queue<int, std::vector<int>, std::greater<int>>{};
    for(auto t = *in++, odd = 0; t--; odd = not odd) {
        odd ? ql.push(*in++)    : qr.push(*in++);
        odd ? qr.push(ql.top()) : ql.push(qr.top());
        odd ? ql.pop()          : qr.pop();
        auto rmed = static_cast<double>(ql.top());
        if (odd) // was odd, we made it even
            rmed = (rmed + static_cast<double>(qr.top())) / 2;
        *out++ = rmed;
    }
    return 0;
}
