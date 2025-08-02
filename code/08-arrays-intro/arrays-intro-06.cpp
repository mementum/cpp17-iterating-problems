#include <algorithm> // std::copy
#include <deque> // std::deque
#include <iostream> // std::cin, std::cout
#include <iterator> // std::istream, std::ostream_iterator, std::inserter
#include <list> // std::vector
#include <stack> // std::stack
#include <vector> // std::vector
#include <type_traits> // std::void_t, std::enable_if ...

// SFINAE to check for I being an Input iterator and delivering a variant
template <typename T, typename Tag>
constexpr bool is_it_tag_v =
    std::is_base_of_v<Tag, typename std::iterator_traits<T>::iterator_category>;

template <typename I>
constexpr bool is_input_v = is_it_tag_v<I, std::input_iterator_tag>;

template <typename O>
constexpr bool is_output_v = is_it_tag_v<O, std::output_iterator_tag>;

template <typename I>
constexpr bool is_bidir_v = is_it_tag_v<I, std::bidirectional_iterator_tag>;

template<typename I, typename O>
constexpr bool io_iterators_v = is_input_v<I> && is_output_v<O>;

template <typename T>
using it_type = typename std::iterator_traits<T>::value_type;

// check if type returned by *I fits into O's operator=
template<typename, typename, typename = void>
constexpr bool i2o_v = false;

template<typename I, typename O>
constexpr bool i2o_v<
    I, O, std::void_t<decltype(std::declval<O>() = *std::declval<I>())>> = true;

// has_begin
template <typename, typename = void>
constexpr bool has_begin_v = false;

template <typename C>
constexpr bool has_begin_v<
    C, std::void_t<decltype(std::declval<C>().begin())>> = true;

// has_rbegin
template <typename, typename = void>
constexpr bool has_rbegin_v = false;

template <typename C>
constexpr bool has_rbegin_v<
    C, std::void_t<decltype(std::declval<C>().rbegin())>> = true;

// has_end
template <typename, typename = void>
constexpr bool has_end_v = false;

template <typename C>
constexpr bool has_end_v<
    C, std::void_t<decltype(std::declval<C>().end())>> = true;

// has_rend
template <typename, typename = void>
constexpr bool has_rend_v = false;

template <typename C>
constexpr bool has_rend_v<
    C, std::void_t<decltype(std::declval<C>().rend())>> = true;

// has_push_front
template <typename, typename = void>
constexpr bool has_push_front_v = false;

template <typename C>
constexpr bool has_push_front_v<
    C,
    std::void_t<
        decltype(
            std::declval<C>().push_front(
                std::declval<typename C::value_type>()))>> = true;

// has_push_back
template <typename, typename = void>
constexpr bool has_push_back_v = false;

template <typename C>
constexpr bool has_push_back_v<
    C,
    std::void_t<
        decltype(
            std::declval<C>().push_back(
                std::declval<typename C::value_type>()))>> = true;

// has_insert
template <typename, typename = void>
constexpr bool has_insert_v = false;

template <typename C>
constexpr bool has_insert_v<
    C,
    std::void_t<
        decltype(
            std::declval<C>().insert(
                std::declval<typename C::const_iterator>(),
                std::declval<typename C::value_type>()))>> = true;

// is_stack
template <typename C>
constexpr bool is_stack_v =
    std::is_base_of_v<std::stack<typename C::value_type>, C>;

// is_container_v
template<typename C>
constexpr bool is_container_v =
    is_stack_v<C> or
    (has_push_front_v<C> and has_begin_v<C> and has_end_v<C>) or
    (has_push_back_v<C> and has_rbegin_v<C> and has_rend_v<C>) or
    (has_insert_v<C> and has_begin_v<C> and has_rbegin_v<C> and has_rend_v<C>);

// c2o (container to output iterator)
template<typename, typename, typename = void>
constexpr bool c2o_v = false;

template<typename O, typename C>
constexpr bool c2o_v<
    O, C,
    std::void_t<
        decltype(
            std::declval<O>() = std::declval<typename C::value_type>())>> = true;

// enable_if
template <typename I, typename O, typename C = void>
using enable_if_ioc = std::enable_if_t<
    io_iterators_v<I, O> and i2o_v<I, O>
    and is_container_v<C> and c2o_v<O, C>>;

// solution
template <
    typename I, typename O, typename C = std::stack<it_type<I>>,
    typename = enable_if_ioc<I, O, C>>
auto
reverse_function(I first, I last, O out) {
    auto oerror = std::ostream_iterator<std::string>(std::cerr, "\n");

    if constexpr (is_bidir_v<I>) {
        // can directly traverse the input backwards, no storage needed
        *oerror++ = "[+]: Bidirectional Iterator for the Input";
        std::copy(
            std::make_reverse_iterator(last),
            std::make_reverse_iterator(first),
            out);
    } else {
        *oerror++ = "[+]: Non-Bidirectional Iterator for the Input";
        auto c = C{}; // container needed to store input and output in reverse

        if constexpr (is_stack_v<C>) {
            *oerror++ = "[+]: Using Stack as container";
            std::for_each(first, last, [&c](const auto &x) { c.push(x); });
            while(not c.empty()) {
                *out = c.top();
                c.pop();
            }

        } else if constexpr (has_push_front_v<C>) {
            *oerror++ = "[+]: Using push_front from container";
            std::copy(first, last, std::front_inserter(c));
            std::copy(c.begin(), c.end(), out);

        } else if constexpr (has_push_back_v<C>) {
            *oerror++ = "[+]: Using push_back from container";
            std::copy(first, last, std::back_inserter(c));
            std::copy(c.rbegin(), c.rend(), out);

        } else if constexpr (has_insert_v<C>) {
            *oerror++ = "[+]: Using insert from container";
            std::copy(first, last, std::inserter(c, c.begin()));
            std::copy(c.rbegin(), c.rend(), out);
        }
    }
}

// main
int
main(int, char *[]) {
    using ptype = int;  // define problem type

    // prepare standard input
    auto sin = std::istream_iterator<ptype>{std::cin};
    auto sin_last = std::istream_iterator<ptype>{};

    [[maybe_unused]] auto N = *sin++; // Get initial (and ignore it) input

#ifdef CASE0 // use a bidirectional iterator, no container in the solution
    // Get things in a vector. // problem iterators are the vector iterators
    auto v = std::vector<ptype>{};
    std::copy(sin, sin_last, std::back_inserter(v));
    auto in = v.begin();
    auto in_last = v.end();
#else // default case, direct input from standard input
    // problem iterators are the standard input iterators
    auto &in = sin;
    auto &in_last = sin_last;
#endif

    using IType = std::decay_t<decltype(in)>;  // iterator type for the templates

    // prepare output and the output iterator type for the templates
    auto out = std::ostream_iterator<ptype>{std::cout, " "};
    using OType = decltype(out);

#ifdef CASE1 // push_back case
    using CType = std::vector<ptype>;
    reverse_function<IType, OType, CType>(in, in_last, out);
#elif defined CASE2 // push_front case
    using CType = std::list<ptype>;
    reverse_function<IType, OType, CType>(in, in_last, out);
#elif defined CASE3 // another push_front case
    using CType = std::deque<ptype>;
    reverse_function<IType, OType, CType>(in, in_last, out);
#else  // default case ... use a stack
    reverse_function(in, in_last, out);
#endif
    return 0;
}
