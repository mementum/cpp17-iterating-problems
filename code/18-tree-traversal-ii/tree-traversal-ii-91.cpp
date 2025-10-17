#include <algorithm> // std::copy_n
#include <chrono> // std::chrono::xx
#include <iostream> // std::cout/cin
#include <iterator> // std::istream/ostream_iterator
#include <memory> // std::unique_ptr
#include <vector> // std::vector

template <class T = int>
class UniquePtrTree {
    struct Node {
        T m_data;
        using NodePtr = std::unique_ptr<Node>;
        NodePtr left, right;
        Node(const T &data) : m_data{data} {};
        auto static create(const T &d) { return std::make_unique<Node>(d); }
    };

    using NodePtr = typename Node::NodePtr;
    NodePtr m_root;

    auto insert(const T &data, NodePtr &node) {
        if(not node) {
            node = Node::create(data);
            return;
        }
        insert(data, data < node->m_data ? node->left : node->right);
    }

    enum class Order { Pre, Post, In };

    template <Order order, typename F>
    auto visit(const F &fvisit, const NodePtr &node) const {
        if (not node)
            return;

        if constexpr (order == Order::Pre) fvisit(node->m_data); // pre
        visit<order>(fvisit, node->left); // visit left
        if constexpr (order == Order::In) fvisit(node->m_data); // inorder
        visit<order>(fvisit, node->right); // visit right
        if constexpr (order == Order::Post) fvisit(node->m_data); // post
    }

public:
    auto insert(const T &data) { insert(data, m_root); };

    template <typename F>
    auto preorder(const F &fvisit) const { visit<Order::Pre>(fvisit, m_root); }
    template <typename F>
    auto postorder(const F &fvisit) const { visit<Order::Post>(fvisit, m_root); }
    template <typename F>
    auto inorder(const F &fvisit) const { visit<Order::In>(fvisit, m_root); }
};

template <typename I, typename F>
auto
solution(I in, F fout, int size) {
    auto tree = UniquePtrTree{};
    while(size--)
        tree.insert(*in++);

    tree.postorder(fout);
}

// Main
int
main(int, char *[]) {
    auto in = std::istream_iterator<int>{std::cin}; // input iterator
    auto out = std::ostream_iterator<int>{std::cout, " "}; // out iter
    auto oerr = std::ostream_iterator<double>(std::cerr, "\n");

    auto reps = 1;
#ifdef REPS
    reps = REPS;
#endif
    auto t = *in++;
    auto v = std::vector<int>(t);
    auto vin = v.begin();
    std::copy_n(in, t, vin); // vin is not invalidated

    auto fout = [&out](const auto &x){ out = x; };
    auto fakeout = [](const auto &){};

    auto start = std::chrono::steady_clock::now();
    solution(vin, fout, t); // to match expected output
    while(--reps)
        solution(vin, fakeout, t); // extra rounds for timing, no output
    auto stop = std::chrono::steady_clock::now();
    auto elapsed_seconds = std::chrono::
        duration_cast<std::chrono::duration<double>>(stop - start).count();
    oerr = elapsed_seconds;
    return 0;
}
