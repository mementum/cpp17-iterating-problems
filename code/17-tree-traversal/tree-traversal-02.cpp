#include <algorithm> // std::copy_n
#include <chrono> // std::chrono::xx
#include <functional> // std::function
#include <iostream> // std::cout/cin
#include <iterator> // std::istream/ostream_iterator
#include <unordered_map> // std::unordered_map
#include <utility> // std::pair
#include <vector> // std::vector

class StdTree {
    static constexpr auto NullTreeVal = int{};
    using Children = std::pair<int, int>;
    static constexpr auto NullChildren = Children{NullTreeVal, NullTreeVal};

    // init tree with empty virtual root
    std::unordered_map<int, Children> m_tree{{NullTreeVal, NullChildren}};
    const int &m_root = m_tree.at(NullTreeVal).second; // ref 2 real root

    auto insert(int data, int node) {
        auto &child = data < node ? m_tree[node].first : m_tree[node].second;
        if (child == NullTreeVal) { // not in tree, add with empty children
            m_tree[child = data] = NullChildren; // set target (left or right)
            return;
        }
        insert(data, child); // already in tree, go deeper
    }

    using FVisit = std::function<void(int)>;

    auto preorder(const FVisit &fvisit, int node) const {
        if (node == NullTreeVal)
            return; // if empty ... will do nothing

        const auto &[left, right] = m_tree.at(node); // get children
        fvisit(node); // preorder ... out root
        preorder(fvisit, left); // visit left
        preorder(fvisit, right); // visit right
    }
public:
    // start always with the virtual root value
    auto insert(int data) { insert(data, NullTreeVal); }
    // start with the reference to the real root
    auto preorder(const FVisit &fvisit) const { preorder(fvisit, m_root); }
};

template <typename I, typename F>
auto
solution(I in, F fout, int size) {
    auto tree = StdTree{};
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
    std::copy_n(in, t, vin); //

    auto fout = [&out](const auto &x){ out = x; };
    auto fakeout = [](const auto &){};

    auto start = std::chrono::steady_clock::now();

    solution(vin, fout, t); // to match expected output
    while(--reps)
        solution(vin, fakeout, t); // extra rounds output nothing

    auto stop = std::chrono::steady_clock::now();
    auto elapsed_seconds = std::chrono::
        duration_cast<std::chrono::duration<double>>(stop - start).count();
    oerr = elapsed_seconds;
    return 0;
}
