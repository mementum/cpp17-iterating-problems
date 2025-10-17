# # Tree Traversal II

**Title**: Tree: Inorder Traversal\
**Link**: <https://www.hackerrank.com/challenges/tree-inorder-traversal>

Let us use the third of the traversal methods, *inorder*, to fix our design, given that
we cannot really fix the performance.

Yes, we need to fix a basic design flaw. When we first addressed the tree traversal
problem we used `int{}` (i.e., `0`) to indicate that a node is a **null** node. This was
not a problem for our solution because we know that problem values will be `> 0`.

The next design used `typename T` as the type and `T{}` as the **null** value. And this
is simply the error repeating itself.

Let us therefore use a feature of *C++17* that we have yet not explored:
`std::optional<T>`. It is a class that manages a value of type `T` but may actually not
contain any value. And that is what will use to have a real **null** value: an
`std::optional` that contains no value.

That is, in fact, the value `std::nullopt` a `constexpr` value of type `std::nullopt_t`,
a value that we will obtain by simply creating an empty `std::optional<T>`.

This is how we now define the data structure of our tree.

```cpp title
--8<-- "{sourcedir}/19-tree-traversal-iii/tree-traversal-iii-01.cpp:49:60"
```

The tree does still take `T` as the type, but we use and store `TNode` which is defined
as `std::optional<T>`. A simple substitution and we suddenly have a real **null** value,
because our `NullTreeVal` is now holding a `constexpr` empty `std::optional<T>`, i.e. an
`std::nullopt` as explained above.

The only small drawback is that we need an extra step to get the value out of our
`std::optional<T>` and that is: using the operator `*` as is we were dereferencing a
pointer or an iterator. However, we only need that when calling the visiting function
`fvisit`.

Notice that the only change is the use of `*node` in `fvisit(*node)`.

```cpp title
--8<-- "{sourcedir}/19-tree-traversal-iii/tree-traversal-iii-01.cpp:64:75"
```

On the other hand, we no longer need to compare against a `NullTreeVal` as we did before,
because our `std::optional<T>` will evaluate to `false` if no value is contained, i.e.,
an `std::nullopt` is contained instead of an instance of type `T`. But we still use it as
a key for the virtual root.

Given that we have added another layer of indirection, let us check the timing of this
solution to see if we have gone yet another order of magnitude away from the optimal
solution.

The timing of our `std::optional` solution

```
$ make 01 rep10000 test05 o2
g++ -std=c++17 -DREPS=10000 -O2 -o build/tree-traversal-iii-01 tree-traversal-iii-01.cpp
0.497966
```

And the one of the `<std::unique_ptr>`-based tree

```
$ make 01 rep10000 test05 o2
g++ -std=c++17 -DREPS=10000 -O2 -o build/tree-traversal-iii-01 tree-traversal-iii-01.cpp
0.0948075
```

We are slightly above `5x`, the multiplier we had when we first measured performance
against a pointer-based tree. Still in that ballpark, which means that using
`std::optional` is not really a headache and has improved our code by letting us use the
full spectrum of values of the type `T`. Recall that we were before blocking `int{}` or
`T{}` as the values to indicate a null node, voiding the presence of those values as
valid data in a node.

## Listings

### Optional-Based Map Tree - 01

```cpp title
--8<-- "{sourcedir}/19-tree-traversal-iii/tree-traversal-iii-01.cpp"
```

### UniquePtr T-Templated/Contsexpress Traversals - 91

```cpp title
--8<-- "{sourcedir}/18-tree-traversal-ii/tree-traversal-ii-91.cpp"
```
