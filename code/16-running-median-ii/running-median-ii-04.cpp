#include <functional> // std::greater
#include <iostream> // std::cout/cin
#include <iomanip> // std::setprecision, ...
#include <iterator> // std::istream/ostream_iterator, std::back_inserter
#include <queue> // std::priority_queue
#include <variant> // std::variant, std::visit
#include <vector> // std::vector

// Main
int
main(int, char *[]) {
    auto in = std::istream_iterator<int>{std::cin}; // input iterator
    auto out = std::ostream_iterator<double>{std::cout, "\n"}; // out iter
    std::cout << std::fixed << std::setprecision(1); // fixed 1 decimal
    using LHeap = std::priority_queue<int>;
    using RHeap = std::priority_queue<int, std::vector<int>, std::greater<int>>;
    using VHeap = std::variant<LHeap, RHeap>;
    auto vql = VHeap{LHeap{}}, vqr = VHeap{RHeap{}};
    const auto visitor = [&in, &out](auto &&ql, auto &&qr) {
        ql.push(*in++); // push to get it sorted
        qr.push(ql.top()); // balance to other side moving top
        ql.pop(); // complete move by removing the moved value
        auto rmed = static_cast<double>(qr.top()); // side we really pushed to
        if (ql.size() == qr.size()) // same size -- use other side too
            rmed = (rmed + static_cast<double>(ql.top())) / 2;
        *out++ = rmed;
    };
    for(auto t = *in++; t--; std::swap(vql, vqr))
        std::visit(visitor, vql, vqr);
    return 0;
}
