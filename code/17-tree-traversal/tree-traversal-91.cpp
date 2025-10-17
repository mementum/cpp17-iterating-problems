#include <algorithm> // std::copy_n
#include <chrono> // std::chrono::xx
#include <functional> // std::function
#include <iostream> // std::cout/cin
#include <iterator> // std::istream/ostream_iterator
#include <memory> // std::unique_ptr
#include <vector> // std::vector

class UniquePtrTree {
    struct Node {
        int m_data;
        using NodePtr = std::unique_ptr<Node>;
        NodePtr left, right;
        Node(int data) : m_data{data} {};
        auto static create(int data) { return std::make_unique<Node>(data); }
    };

    using NodePtr = Node::NodePtr;
    NodePtr m_root;

    auto insert(int data, NodePtr &node) {
        if(not node) {
            node = Node::create(data);
            return;
        }
        insert(data, data < node->m_data ? node->left : node->right);
    }

    using FVisit = std::function<void(int)>;

    auto preorder(const FVisit &fvisit, const NodePtr &node) const {
        if (not node)
            return;

        fvisit(node->m_data);
        preorder(fvisit, node->left);
        preorder(fvisit, node->right);
    }
public:
    auto insert(int data) { insert(data, m_root); }
    auto preorder(const FVisit &fvisit) const { preorder(fvisit, m_root); }
};

template <typename I, typename F>
auto
solution(I in, F fout, int size) {
    auto tree = UniquePtrTree{};
    while(size--)
        tree.insert(*in++);

    tree.preorder(fout);
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

    auto fout = [&out](auto x){ out = x; };
    auto fakeout = [](auto){};

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
