# # Tree Traversal II

**Title**: Tree: Postorder Traversal\
**Link**: <https://www.hackerrank.com/challenges/tree-postorder-traversal>

In the previous challenge we stated we would address several tree traversal methods with
a single codebase. We therefore extend the latest solution to do it.

We will also make our `StdTree` a bit more generic as it will:

  - Take a `typename T` instead of simply taking `int` as the type for the nodes.

  - The visit function will also be a template parameter.

That will push us to implement *SFINAE* directly to check that:

  - The type `T` supports the operators `<`, `==` and `!=`, the ones we are using for
    different parts of the code to chose alternative actions.

  - The visit function type `F` can take a single `T` as a parameter.

Although we would be looking at implementing only a `postorder` traversal method, we will
also add `inorder`, because we will use the pattern to improve our implementation.

The *SFINAE* checks for operator support follow this pattern.

```cpp title
--8<-- "{sourcedir}/18-tree-traversal-ii/tree-traversal-ii-01.cpp:11:16"
```

The visiting function check is also no stranger, with `std::void_t` and `std::invoke_result`
taking the load of the work.

```cpp title
--8<-- "{sourcedir}/18-tree-traversal-ii/tree-traversal-ii-01.cpp:37:42"
```

All those small checks are wrapped in custom `enable_if_xxx` definitions and later
applied to the corresponding functions.

Let us concentrate on the traversal methods. First the core or `preorder`

```cpp title
--8<-- "{sourcedir}/18-tree-traversal-ii/tree-traversal-ii-01.cpp:64:66"
```

And then `postorder` and `inorder`.

```cpp title
--8<-- "{sourcedir}/18-tree-traversal-ii/tree-traversal-ii-01.cpp:74:76"
```

```cpp title
--8<-- "{sourcedir}/18-tree-traversal-ii/tree-traversal-ii-01.cpp:84:86"
```

And the pattern emerges:

  - We recurse using the same method to the left and to the right

  - Only when the `fvisit(node)` call is executed changes

    - `preorder`: before visiting the lower nodes

    - `postorder`: after visiting the lower nodes

    - `inorder`: after visiting the left lower nodes and before visiting the right lower
      ones

This means that we actually only move around the position of the `fvisit(node)` call and
the rest is just a node visiting thing. That is our pattern to go for more.

## Constexpressing the Traversals

With the aforementioned pattern we can apply our *C++17* beloved `if constexpr` to choose
where to place the `fvisit` call.

The external interfaces for the traversal methods will all rely on a single internal
interface named `visit`.

```cpp title
--8<-- "{sourcedir}/18-tree-traversal-ii/tree-traversal-ii-02.cpp:85:90"
```
Each public method specializes `visit` with an internal `enum` that properly defines the
names `Pre`, `In` and `Post`. That specialization is the key to let `if constexpr` place
the `fvisit(node)` call.

```cpp title
--8<-- "{sourcedir}/18-tree-traversal-ii/tree-traversal-ii-02.cpp:58:71"
```

As we know this generates as many different methods in the background as we invoke
`visit` with different template parameters. But from a practical point of view we have
implemented a single method capable of working as three different methods.

## Futile Attempt At Performance

Now that we have proper external and internal interfaces, let us try to improve
performance. It is probably going to be a futile attempt because we have a `10x`
performance difference. We will, however, introduce a small change to see if something
changes. Recall that Bon Jovi sang: *"The more things change the more they stay the
same"* and our performance is therefore doomed to stay the same.

In our solution we are using `std::pair<int, int>` (or `<T, T`> in the templated version)
to hold the children and have then to access `.first` and `.second` to choose the left
and right children node. But because `std::pair` can handle different types, there has to
be a lot of machinery to account for that. Let us therefore simplify that: we will use
`std::array<T, 2>` to have a fixed array size assignment and no type magic.

We introduce an *enum* to properly access `Node::Left` and `Node::Right`, although we
only need to keep the initial reference to the real root as a child of the virtual root.
This is so because we will still use *Structured Bindings* and the comparison `<` of `T`
types when inserting to choose the position.

Here is how the `Children` now look like.

```cpp title
--8<-- "{sourcedir}/18-tree-traversal-ii/tree-traversal-ii-03.cpp:48:53"
```

Accessing both the `left` and `right` node at the same time does not change.

```cpp title
--8<-- "{sourcedir}/18-tree-traversal-ii/tree-traversal-ii-03.cpp:61:66"
```

But we put the `operator []` of `std::array` to good use when selecting the child to
insert things into (or create a new node)

```cpp title
--8<-- "{sourcedir}/18-tree-traversal-ii/tree-traversal-ii-03.cpp:74:75"
```

Let us check if the performance has actually improved.

```
$ make 03 test05 rep10000 o2
g++ -std=c++17 -DREPS=10000 -o build/tree-traversal-ii-03 tree-traversal-ii-03.cpp
0.394339
```

Not really, i.e., no, there has been no improvement at all, when considering *"orders or
magnitude"*. It would really have been a miracle, had it happened. Let us think about it:
an `std::unordered_map` is based on a hash table and that means that we are implementing
a container, a tree, on top of another container. An `std::map` is for example
implemented atop a red-black tree and that is already a good indication that implementing
a tree on top of another tree (or a similar structure) is going to hurt the performance
of our solution.

But if we compare that timing with the timing of our `01` solution (still `std::pair`
based) there is an average advantage of `20` milliseconds for the test with `10,000`
repetitions. Not much, but it shows that `std::pair` is giving us a small leading edge.

## Listings

Let us list the code of the three solutions we presented above.

### T-Template Map-Based Tree - 01

```cpp title
--8<-- "{sourcedir}/18-tree-traversal-ii/tree-traversal-ii-01.cpp"
```

### Constexpr Traversals - 02

```cpp title
--8<-- "{sourcedir}/18-tree-traversal-ii/tree-traversal-ii-02.cpp"
```

### Removing std::pair - 03

```cpp title
--8<-- "{sourcedir}/18-tree-traversal-ii/tree-traversal-ii-03.cpp"
```

### UniquePtr T-Templated/Contsexpress Traversals - 91

```cpp title
--8<-- "{sourcedir}/18-tree-traversal-ii/tree-traversal-ii-91.cpp"
```
