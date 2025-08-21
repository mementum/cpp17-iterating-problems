# Running Median II

**Title**: Find the Running Median\
**Link**: <https://www.hackerrank.com/challenges/find-the-running-median/>

# The Complexity Conundrum

As we saw with the previous examples, *"Complexity Killed The Cat"*, because as soon as
we did not have an efficient insertion method to keep the container sorted, even our
*Floating Iterator* approach was not enough. Even if it reduced the operations to
calculate the running media to a single iterator increment/decrement operation and not
always.

The only other way to reduce complexity may be to look at a container hosting a
*"RandomAccessIterator"*. This is the only iterator that can *"randomly"* jump and
therefore must not be incremented `n` times (expensive) to reach the expected
destination. A single operation suffices.

Let us give `std::vector<T>` a try. The problem that one can foresee is clear:

  - An insertion is expensive.

  - Re-allocations and copies to fit new and old elements in a contiguous array.

Here is the code and it beats all test cases, when not using `CASE1`.

```cpp title
--8<-- "{sourcedir}/16-running-median-ii/running-median-ii-01.cpp"
```

As it was the case with the `std::list` implementation we need to look for the insertion
point to keep things always sorted. The *"good case"* is to use `std::lower_bound`. If you
wonder why that is the case, it is because it uses the goodies from
*RandomAccessIterator* to gives us the insertion point by repeatedly partitioning the
range, until it finds its target.

The *"bad case"* mimics what we did with `std::list` by using `std::find` to get the
insertion point. It works but it is again so expensive that complexity-limited test cases
will be failed.

After that the contiguous nature of `std::vector` offers us another advantage: we do not
need to float an iterator along the running median. Using the `operator []` is enough to
get to the running media point in just a single operation and to the next position in
cases where the size is even (after the insertion).

Having to move items to make place for the inserted element and to reallocate and
copy elements if needed is not enough to kill the benefits of the low complexity of the
*RandomAccessIterator* that `std::vector` features.

## Living On The Heap

This is not a matter of a second though and also not having second thoughts about the
previous implementations. The breadcrumbs leading to this problem on *HackerRank* show
this: *"Data Structures > Heap > Find the Running Median"*. It may well be that we missed
the entire point of the challenge by thinking too much outside of the box with our
`std::multiset` solution and focusing then on the complexity with the additional
`std::vector` approach.

The *Heap-Wish* is fulfilled with a new `std::priority_queue` implementation.

```cpp title
--8<-- "{sourcedir}/16-running-median-ii/running-median-ii-02.cpp"
```

The queue acts like a heap, keeping always the largest element at the top. Unless, of
course, a comparison function like `std::greater<T>` is given to keep the minimum element
at the top.

Combining a left heap (max-at-the-top) and a right heap (min-at-the-top) we can simulate
the *Floating Iterator* approach, because we have direct access to the elements that
allow us to calculate the running median.

```cpp title
--8<-- "{sourcedir}/16-running-median-ii/running-median-ii-02.cpp:17:19"
```

These three lines are the key. Push to one heap, depending on parity, and move (`top` +
`pop`) to the other heap to keep them balanced (in *"odd"* cases the left heap has one
element more as the right heap)

The one-liners are an *"odd"* construction with the formatting showing that we perform
the same operations during each round, but with the roles of the heaps swapped. We would
ideally use polymorphism to avoid having to know on which heap we operate. However,
`std::priority_queue<int>` and its counterpart
`std::priority_queue<int, std::vector<int>, std::greater<int>>` are different types. And
we are not in the business of using `void *` to duck types.

## Type Erasure for Heaps

What we need is to create our own inheritance chain to make the types polymorphic, i.e.,
create a wrapper with templates that mimic the needed interface and then create our
templated *"subclasses"*.

```cpp title
--8<-- "{sourcedir}/16-running-median-ii/running-median-ii-03.cpp:9:25"
```

The only difference with regards to usual implementations is that the base class takes a
template parameter too. That is to avoid having to specify the type taken by `push` and
returned by `top`. Luckily we can directly calculate `T` in the `MyHeap` implementation,
by looking at `PrioQ::value_type`. Both our heaps use the same `value_type` and will
therefore be subclasses of our `Heap<T>` abstract base class.

```cpp title
--8<-- "{sourcedir}/16-running-median-ii/running-median-ii-03.cpp:33:42"
```

And the polymorphic implementation can happen. We store two pointers in an `std::array`
and during each iteration we select the heap that gets the new value pushed and than
balances itself to the other heap.

The one-liners have become real. However, the calculation of the running media does still
reference the left and right heaps.

This solution, as it previous non-polymorphic implementation, passes all the tests.
Mission accomplished.

```cpp title
--8<-- "{sourcedir}/16-running-median-ii/running-median-ii-03.cpp"
```
