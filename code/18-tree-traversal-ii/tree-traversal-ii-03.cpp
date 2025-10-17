#include <algorithm> // std::copy_n
#include <array> // std::array
#include <iostream> // std::cout/cin
#include <iterator> // std::istream/ostream_iterator
#include <unordered_map> // std::unordered_map
#include <chrono> // std::chrono::xx
#include <vector> // std::vector
#include <type_traits> // std::enable_if, std::invoke_result, std::void_t

// SFINAE for the Tree Type (operator support)
template<typename, typename = void>
constexpr bool op_lt_v = false;

template<typename T>
constexpr bool
op_lt_v<T, std::void_t<decltype(std::declval<T>() < std::declval<T>())>> = true;

template<typename, typename = void>
constexpr bool op_ne_v = false;

template<typename T>
constexpr bool
op_ne_v<T, std::void_t<decltype(std::declval<T>() != std::declval<T>())>> = true;

template<typename, typename = void>
constexpr bool op_eq_v = false;

template<typename T>
constexpr bool
op_eq_v<T, std::void_t<decltype(std::declval<T>() == std::declval<T>())>> = true;

template <typename T>
using enable_if_T_ops =
    std::enable_if_t<op_lt_v<T> and op_ne_v<T> and op_eq_v<T>>;

// SFINAE: Visiting Function
template<typename, typename, typename = void>
constexpr bool f_visit = false;

template<typename F, typename T>
constexpr bool
f_visit<F, T, std::void_t<std::invoke_result_t<F, T>>> = true;

template <typename F, typename T>
using enable_if_fvisit = std::enable_if_t<f_visit<F, T>>;

// Tree Class
template <typename T = int, typename = enable_if_T_ops<T>>
class StdTree {
    static constexpr auto NullTreeVal = T{};
    enum Node { Left = 0, Right = 1, Total = 2 };
    using Children = std::array<T, Node::Total>;
    static constexpr auto NullChildren = Children{NullTreeVal, NullTreeVal};

    // init tree with empty virtual root
    std::unordered_map<T, Children> m_tree{{NullTreeVal, NullChildren}};
    const T &m_root = m_tree.at(NullTreeVal)[Node::Right]; // ref 2 real root

    enum class Order { Pre, Post, In };

    template <Order order, typename F, typename = enable_if_fvisit<F, T>>
    auto visit(const F &fvisit, const T &node) const {
        if (node == NullTreeVal)
            return; // if empty ... will do nothing

        const auto &[left, right] = m_tree.at(node); // get children
        if constexpr (order == Order::Pre) fvisit(node); // pre -> root
        visit<order>(fvisit, left); // visit left
        if constexpr (order == Order::In) fvisit(node); // inorder -> root
        visit<order>(fvisit, right); // visit right
        if constexpr (order == Order::Post) fvisit(node); // post -> root
    }

    void insert(const T &data, const T &node) {
        auto &child = m_tree.at(node)[not (data < node)];
        if (child != NullTreeVal)
            return insert(data, child); // already in tree, go deeper
        // not in tree, add it with default empty children
        m_tree[child = data] = NullChildren; // set target (left or right ref)
    }

public:
    // start always with the virtual root value
    auto insert(const T &data) { insert(data, NullTreeVal); }
    // start with the reference to the real root
    template <typename F, typename = enable_if_fvisit<F, T>>
    auto preorder(const F &fvisit) const { visit<Order::Pre>(fvisit, m_root); }
    template <typename F, typename = enable_if_fvisit<F, T>>
    auto inorder(const F &fvisit) const { visit<Order::In>(fvisit, m_root); }
    template <typename F, typename = enable_if_fvisit<F, T>>
    auto postorder(const F &fvisit) const { visit<Order::Post>(fvisit, m_root); }
};

// Solution Function
template <typename I, typename F>
auto
solution(I in, F fout, int size) {
    auto tree = StdTree{};
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
