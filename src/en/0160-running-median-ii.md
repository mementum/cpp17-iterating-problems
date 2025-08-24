# Running Median II

**Title**: Find the Running Median\
**Link**: <https://www.hackerrank.com/challenges/find-the-running-median/>

## The Complexity Conundrum

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

## Type Erasure Polymorphism

What we need is to create our own inheritance chain to make the types polymorphic, i.e.,
create a wrapper with templates that mimic the needed interface and then create our
templated *"subclasses"*. This is called *Type Erasure*, although nobody is erasing
actually anything. The type is being hidden behind another type. This new type does
usually present the same interface, but it can actually adapt interfaces effectively
mixing real different types. Our types are only different because of the comparison
function, `std::less` vs `std::greater`. For all intent and purposes they are polymorphic
and we use *Type Erasure* to present them as such.

```cpp title
--8<-- "{sourcedir}/16-running-median-ii/running-median-ii-03.cpp:8:32"
```

The only difference with regards to usual implementations is that the base class takes a
template parameter `T`. That is to avoid having to specify the type taken by `push` and
returned by `top`. Luckily we can directly calculate `T` in the `MyHeap` implementation,
by looking at `PrioQ::value_type`. Both our heaps use the same `T`, and `int`, and will
therefore be subclasses of our `Heap<T>` abstract base class. Notice that we have there a
`get_basepointer` method, so that any subclass can get a pointer to `Heap<T>` to work to
easily work with polymorphism.

```cpp title
--8<-- "{sourcedir}/16-running-median-ii/running-median-ii-03.cpp:38:50"
```

We create the heaps and get the pointers to them. The same `auto` allows us to get the
pointer for both because we are getting a pointer to `Heap<int>`, i.e.: the same type.
This is where polymorphism starts to show up. As it does in the `for` loop where we use
`std::swap(qlp, qrp)`, to swap the pointers in the variables. Were they not compatible,
we could not do that.

The algorithm operates now without knowing if we are in an `odd` or `even` case and
simply pushes to one heap, sorting the values, and the balances whatever is at the top to
the other side. Instead of keeping tabs with the parity of the count, we simply compare
the size of the polymorphic queues to know if we need just one value for the running
median or the average of two.

This solution, as it previous non-polymorphic implementation, passes all the tests.
Mission accomplished.

```cpp title
--8<-- "{sourcedir}/16-running-median-ii/running-median-ii-03.cpp"
```

## Variant (Pseudo-)Polymorphism

The question is whether we really need polymorphism with *C++17* or we can use modern
facilities to achieve the same effect without resorting to *tricks* like creating our
own hierarchy to achieve *Type Erasure*.

We are only really interested in reaching the `push`, `top` and `top` methods of our
heaps, the methods of the based `std::priority_queue` instances we use as heaps. Recall
that when were addressing the [*"Types, Types, Types
"*](0050-types-types-types.md#060-types) challenge, we resorted to using `std::variant`,
a newcomer in *C++17* described as a type-safe union that can therefore hold different
types, always one at time.

That is going to be our key cornerstone to implement the *variant-polymorphism*: we will
have an `std::variant` holding the types of both heaps, the left one (max-at-the-top) and
the right one (min-at-the-top). We will then have two variants, each holding an instance
of the heaps and we will use them to push, balance, calculate the running media and then
swap them as we did before. Without having to define a chain hierarchy to implement *Type
Erasure*.

```cpp title
--8<-- "{sourcedir}/16-running-median-ii/running-median-ii-04.cpp"
```

The only drawback in the code above, if we could say so, is that the
push-balance-calculate has to be placed in a *lambda expression*. This goes into
`std::visit`, the functionality that will feed our heaps to the algorithm. Using
`std::visit` is the key, because it passes the type being held in the `std::variant`
without forcing us to know it beforehand or using the index.

Given that a *lambda expression* can declare its parameters to be of type `auto`, the
combination is ideal.

The algorithm is exactly the same as in the previous solution but no pointers are
involved.

/// pagebreak ///
