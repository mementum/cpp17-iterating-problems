# Tree Traversal

**Title**: Tree: qqqqqqqPreorder Traversal\
**Link**: <https://www.hackerrank.com/challenges/tree-preorder-traversal>

Although we refer above to the *"Preorder Traversal"* problem, there is actually an
entire family of traversal problems: preorder, postorder, in-order, top-view order,
height-of-a-tree, that we will be addressing with a single generic codebase. That is what
the magic of conditional compilation can help us with.

## The Standard Binary Tree Structure

The problem as presented has to be solved by filling out a single function that receives
a pre-constructed binary tree with the standard tree-pointer approach. This can be seen
in the *C++11* and *C++14* editors, with nothing but the `main` function in the *C++20*
version. Remember that is the reason we chose to use *Hackerrank* problems: we could take
on the entire problem, including the input and the output.

```cpp title="Tree Standard Structure"
class Node {
    public:
        int data;
        Node *left;
        Node *right;
        Node(int d) {
            data = d;
            left = NULL;
            right = NULL;
        }
};
```

Pointers and `NULL` values are going to be used for insertion, look-up and everything one
may conceive. The questions being: *do we really need pointers?*, *do we really need to
define such a structure?*.

And considering we are using *Modern C++* and *C++17* the answer is: **No**. It is not
that we are going to reinvent the wheel, because a tree can also be seen and manipulated
as an array. That means this problem has been solved by many generations of coders. We
are simply going to tackle it with the tools of the standard library and the presents of
*C++17*, wherever possible.

## A flat map approach

The input goes from `1` to `n`, but we do not know in which order the integers are going
to be be delivered and allocating up to `n` positions of memory is, obviously, a no-go.
We are going to use a mapping structure to hold the `parent => children` relationship,
i.e., `key => value`. We need no specific ordering just to keep a reference to the `root`
value to know where to start our traversal.

Let us see the bits holding the data of our tree.

```cpp title
--8<-- "{sourcedir}/17-tree-traversal/tree-traversal-01.cpp:7:13"
```

A binary tree has two children: left and right. That is why the `std::pair<int, int>`
seems to be ideally suited to hold the values. We need a `NULL` equivalent value and
given the lowest input value will be `1` we can use the default `int` value, that we get
by using a default constructor.

Every time we create a node we need `NULL` children and that is why create a static pair
holding null-children.

The real data is held in an `std::unordered_map<int, Children>`, with the addition of
`int m_root`, our member attribute holding the top of the tree.

This approach is giving us two major advantages over a standard tree structure:

  - No manual memory management. The `std::unordered_map` and the `std::pair` are doing
    all of it for us.

  - If we ever wanted to quickly find out the values of the children of any given parent,
    we could deliver without having to first find out where that parent is located.

To insert in the tree we use a public and private interface, both methods with the same
name, by simply using overloading. The public part:

```cpp title
--8<-- "{sourcedir}/17-tree-traversal/tree-traversal-01.cpp:36:41"
```

And the private overload.

```cpp title
--8<-- "{sourcedir}/17-tree-traversal/tree-traversal-01.cpp:15:21"
```

## Preorder Traversal

Having the tree structure in the hand we can solve our traversal problem. We are going to
use the same private-public-overload approach and we of course manage the input and
output using iterators. *"Preorder"* implies that we first *visit* the data of the node
and, and then the left descendants. Once that is over we go for the right descendants. To
easily access the left and right node we use a *Structured Binding* construct.

We are using a recursive approach for clarity and because *C++* compilers will perform
tail call optimization when compiling for speed. I.e.: a recursive call will be changed
to a jump to avoid adding new frames to the stack, what could end up in a stack overflow
error.

Here is the public interface

```cpp title
--8<-- "{sourcedir}/17-tree-traversal/tree-traversal-01.cpp:43:43"
```

And the private.

```cpp title
--8<-- "{sourcedir}/17-tree-traversal/tree-traversal-01.cpp:24:34"
```

## Our "Standard" Performance

We have been so far creating solutions to the problems using the standard library as much
as possible and creating the tree structure with an `std::unordered_map` has been no
exception. However, we have not asked ourselves if this improves the performance and if
it does not, how much are we impacting that performance. We are, of course, profiting
from nice construction and syntax and avoiding memory management.

Let us therefore measure the performance of a pointer based tree vs our implementation.
We will still add a virtual root, to slight simplify our code. I.e., we add an empty root
at the top and all `insert` operations, including that of the real root, take place after
the top virtual root.

The methods that can take `const` get it and that means we remove the `operator []`. This
is so because in maps that operator will add a value if key being sought is not present.
In contrast, the method `at` will not.

To measure performance we will use `std::chrono` to start an `std::chrono::steady_clock`
and calculate the duration. We also copy all input values to an `std::vector` to be able
to repeat the test and avoid microseconds measurements.

Hence, the most important part is how we modify `main` to get the timing of our test.

```cpp title
--8<-- "{sourcedir}/17-tree-traversal/tree-traversal-02.cpp:56:86"
```

This is the structure we will keep for all tests. The `solution` function is simple and
just loads the values and executes the traversal.

```cpp title
--8<-- "{sourcedir}/17-tree-traversal/tree-traversal-02.cpp:46:54"
```

The only other thing that really changes is the initialization of the tree with the
virtual root and the initialization of a reference to where the real root will be stored.

```cpp title
--8<-- "{sourcedir}/17-tree-traversal/tree-traversal-02.cpp:16:17"
```

The code of our `02` solution will be trying to match the performance of this pointer
based code.

```cpp title
--8<-- "{sourcedir}/17-tree-traversal/tree-traversal-90.cpp:8:44"
```

With `main` and `solution` being as presented above.

## The Performance War

Not much of a war. Just take a look at the values executed with `10,000` repetitions of
filling up the tree with values and executing the traversal.

The standard code run (for the sake of brevity, parts of the actual output are left out)

```
$ make 02 test05 rep10000
g++ -std=c++17 -DREPS=10000 -o build/tree-traversal-02 tree-traversal-02.cpp
2.89785
```

And the same execution with the pointer based tree.

```
$ make 90 test05 rep10000
g++ -std=c++17 -DREPS=10000 -o build/tree-traversal-90 tree-traversal-90.cpp
0.289652
```

This really deserves a WTF!! Pointers are `10x` faster, i.e., an order of magnitude
faster. Both runs have `10,000` reps and use the input and output of test case `05`, a
tree with `445` nodes.

Now we know: even if our solution seems nice, elegant, frees us from manual memory
management and offers us for sure more perks, it is **slow**. Luckily, our *C++17*
exercise does not revolve around maximum performance. We are looking to use standard
features and specifically new features, those that can also help in many real life
situations.

We will actually come to that with some future versions of our tree traversal solutions.

## The Optimized Performance War

The tests we executed above were compiled with no optimization. Let us see if the
compiler can optimize our code by using the target `o2`, so we compile with `-O2`.

```
$ make 02 test05 rep10000 o2
g++ -std=c++17 -DREPS=10000 -O2 -o build/tree-traversal-02 tree-traversal-02.cpp
0.446231
```

Ooops! That puts the performance of our map-based tree in the ballpark of pointers.

```
$ make 90 test05 rep10000 o2
g++ -std=c++17 -DREPS=10000 -O2 -o build/tree-traversal-90 tree-traversal-90.cpp
0.0887391
```

Not anymore after we also compile the pointer-based solution with `-O2`. In any case we
have reduced the performance multiplier from `10x` to `5x`.

## Switching to UniquePtr

One of our goals is to use as many of the standard, and modern, facilities that *C++*
offers us. Instead of manually managing the pointers with `new` and `delete`, we will let
`std::unique_ptr<T>` take care of the management, after creating the object with
`std::make_unique<T>(Args... &args)`.

Notice how we have an `auto static create(int data)` function in the original pointer
based implementation.

```cpp title
--8<-- "{sourcedir}/17-tree-traversal/tree-traversal-90.cpp:14:14"
```

That means that we only have to change who to return an `std::make_unique<Node>` here and
where we use `create` remains unchanged. Here is the change in the implementation

```cpp title
--8<-- "{sourcedir}/17-tree-traversal/tree-traversal-91.cpp:15:15"
```

As easy as that. Because we no longer have the need to manage the lifetime of the pointer
held in the `std::unique_ptr<Node>` we can remove the destructors where `delete` is
active. Here:

```cpp title
--8<-- "{sourcedir}/17-tree-traversal/tree-traversal-90.cpp:15:15"
```

And here:

```cpp title
--8<-- "{sourcedir}/17-tree-traversal/tree-traversal-90.cpp:43:43"
```

Wherever we had a `Node *`, we know use an alias named `NodePtr` (obvious name choice)
defined as:

```cpp title
--8<-- "{sourcedir}/17-tree-traversal/tree-traversal-91.cpp:11:11"
```

After making those changes, let us go for the performance.

```
$ make 91 test05 rep10000 o2
g++ -std=c++17 -DREPS=10000 -O2 -o build/tree-traversal-90 tree-traversal-91.cpp
0.0896148
```

That is. We have a `1x` (it's almost a perfect timing match) performance multiplier,
i.e.,: we can confidently dropped raw-pointer use and work entirely with
`std::make_unique<T>` with no fear of having an impact in the performance of the
solution.

## Listings

### 01 - Map-Based Tree Implementation

```cpp title
--8<-- "{sourcedir}/17-tree-traversal/tree-traversal-01.cpp"
```

### 02 - Map-Based Tree Implementation with Timing

```cpp title
--8<-- "{sourcedir}/17-tree-traversal/tree-traversal-02.cpp"
```

### 03 - Pointer-Based Tree Implementation

```cpp title
--8<-- "{sourcedir}/17-tree-traversal/tree-traversal-90.cpp"
```

### 03 - UniquePtr-Based Tree Implementation

```cpp title
--8<-- "{sourcedir}/17-tree-traversal/tree-traversal-91.cpp"
```

/// pagebreak ///
