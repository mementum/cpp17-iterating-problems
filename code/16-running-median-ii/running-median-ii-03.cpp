#include <functional> // std::greater
#include <iostream> // std::cout/cin
#include <iomanip> // std::setprecision, ...
#include <iterator> // std::istream/ostream_iterator
#include <queue> // std::priority_queue
#include <vector> // std::vector

// Type Erasure Idiom for the Heap
template<typename T>
struct Heap {
    virtual ~Heap() = default;

    auto *get_basepointer() { return this; }

    // abstract interface
    virtual void push(const T &) = 0;
    virtual T top() const = 0;
    virtual void pop() = 0;
    virtual size_t size() const = 0;
};

template<typename T, template <typename> typename Comp = std::less>
struct MyHeap : Heap<T> {
    using PrioQ = std::priority_queue<T, std::vector<T>, Comp<T>>;
    PrioQ m_prioq;

    void push(const T &x) override { m_prioq.push(x); }
    T top() const override { return m_prioq.top(); }
    void pop() override { m_prioq.pop(); }
    size_t size() const override { return m_prioq.size(); }
};

// Main
int
main(int, char *[]) {
    auto in = std::istream_iterator<int>{std::cin}; // input iterator
    auto out = std::ostream_iterator<double>{std::cout, "\n"}; // out iter
    std::cout << std::fixed << std::setprecision(1); // fixed 1 decimal
    auto ql = MyHeap<int>{}; // left heap (max at top)
    auto qr = MyHeap<int, std::greater>{}; // right heap (min at top)
    auto *qlp = ql.get_basepointer(), *qrp = qr.get_basepointer();
    for(auto t = *in++; t--; std::swap(qlp, qrp)) {
        qlp->push(*in++); // push to get it sorted
        qrp->push(qlp->top()); // balance to other side moving top
        qlp->pop(); // complete move by removing the moved value
        auto rmed = static_cast<double>(qrp->top()); // side we really pushed to
        if (qlp->size() == qrp->size()) // same size -- use other side too
            rmed = (rmed + static_cast<double>(qlp->top())) / 2;
        *out++ = rmed;
    }
    return 0;
}
