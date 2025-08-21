#include <array> // std::array
#include <functional> // std::greater
#include <iostream> // std::cout/cin
#include <iomanip> // std::setprecision, ...
#include <iterator> // std::istream/ostream_iterator, std::back_inserter
#include <queue> // std::priority_queue
#include <vector> // std::vector

// Type Erasure Idiom for the Heap
template<typename T>
struct Heap {
    virtual void push(const T &) = 0;
    virtual T top() const = 0;
    virtual void pop() = 0;
    virtual ~Heap() = default;
};

template<typename PrioQ, typename T = typename PrioQ::value_type>
struct MyHeap : Heap<T> {
    using value_type = T;
    PrioQ prioq;
    void push(const T &x) override { prioq.push(x); }
    T top() const override { return prioq.top(); }
    void pop() override { prioq.pop(); }
};

// Main
int
main(int, char *[]) {
    auto in = std::istream_iterator<int>{std::cin}; // input iterator
    auto out = std::ostream_iterator<double>{std::cout, "\n"}; // out iter
    std::cout << std::fixed << std::setprecision(1); // fixed 1 decimal
    auto ql = MyHeap<std::priority_queue<int>>{}; // left heap / right heap
    auto qr =
        MyHeap<std::priority_queue<int, std::vector<int>, std::greater<int>>>{};
    using HeapBase = Heap<decltype(ql)::value_type>;
    auto qa = std::array<HeapBase *, 2>{&qr, &ql};
    for(auto t = *in++, odd = 0; t--; odd = not odd) {
        auto &qlv = *qa[odd], &qrv = *qa[not odd];
        qlv.push(*in++);
        qrv.push(qlv.top());
        qlv.pop();
        auto rmed = static_cast<double>(ql.top());
        if (odd) // was odd, we made it even
            rmed = (rmed + static_cast<double>(qr.top())) / 2;
        *out++ = rmed;
    }
    return 0;
}
