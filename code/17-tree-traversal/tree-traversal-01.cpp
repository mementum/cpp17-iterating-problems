#include <functional> // std::function
#include <iostream> // std::cout/cin
#include <iterator> // std::istream/ostream_iterator
#include <unordered_map> // std::unordered_map
#include <utility> // std::pair

class StdTree {
    static constexpr auto NullTreeVal = int{};
    using Children = std::pair<int, int>;
    static constexpr auto NullChildren = Children{NullTreeVal, NullTreeVal};

    std::unordered_map<int, Children> m_tree;
    int m_root = NullTreeVal;

    auto insert(int data, int &node) {
        auto &child = data < node ? m_tree[node].first : m_tree[node].second;
        if (child == NullTreeVal) { // not in tree, add with empty children
            m_tree[child = data] = NullChildren; // set target (left or right ref)
            return;
        }
        insert(data, child); // already in tree, go deeper
    }

    using FVisit = const std::function<void(int)>;

    auto preorder(const FVisit &fvisit, int node) {
        if (node == NullTreeVal)
            return; // if empty ... will do nothing

        const auto &[left, right] = m_tree[node]; // get children
        fvisit(node); // preorder ... out root
        preorder(fvisit, left); // visit left
        preorder(fvisit, right); // visit right
    }
public:
    auto insert(int data) {
        if (m_root != NullTreeVal) // root already defined?
            return insert(data, m_root); // yes, insert

        m_tree[m_root = data] = NullChildren; // set m_root with empty children
    }

    auto preorder(const FVisit &fvisit) { preorder(fvisit, m_root); }
};

// Main
int
main(int, char *[]) {
    auto in = std::istream_iterator<int>{std::cin}; // input iterator
    auto out = std::ostream_iterator<int>{std::cout, " "}; // out iter

    auto tree = StdTree{};
    for (auto t = *in++; t--;) // get number of nodes and count down
        tree.insert(*in++); // insert nodes

    tree.preorder([&out](const auto &x){ out = x; });
    return 0;
}
