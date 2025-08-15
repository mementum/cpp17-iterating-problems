# The Angry Professor - Reloaded

**Title**: Angry Professor\
**Link**: <https://www.hackerrank.com/challenges/angry-professor>

## Adding to the "x_of" Standard Family

Let us remain with the *"Angry Professor"* and on a new second thought moment let us
consider if we do not need our custom iterator at all. We have counted, partitioned and
iterated, but we have missed the entire point. Our goal is to count we have *"at least k
students of n"*, with `n` being the entire range, of course.

And the standard library has three algorithms with the `_of_`, namely: `std::all_of,
`std::any_of` and `std::none_of`. It is obvious that as we needed, these algorithms can
break out early if the condition that is being tested fails. Unfortunately they do not
test for `n` elements or *"at least"* `n`.

Let us roll our own `n_of`. Here is the main code skipping all *SFINAE* definitions we
already know so well.

```cpp title
--8<-- "{sourcedir}/14-angry-prof-again/angry-prof-again-01.cpp:41:56"
```

Because calculating the `std::distance` between `first` and `last` can be expensive for
non *RandomAccessIterator* instances, we add an `if constexpr` guard, to only add the
calculation, and later the optimization, for that type of iterators.

The idea is that if the distance to the end is less than the amount of needed items to
find `n` items, we can already break early out.

The algorithm is of course simple: see if we have exactly `n` elements as determined by
function `f`, a unary predicate returning something convertible to `bool`. Notice that we
have a parameter `bool at_least = false`. If we set it to `true`, the algorithm will
return `true` as soon as the n^th^ element has been found. We do not care if more `true`
matches could uncover more items, because we are only asking for *"at least"* `n`
matches.

Our problem can be solved as soon as we have enough, i.e., *"at least"*, students
arriving early, If more arrive, the more the merrier, but it does not have any
implication in deciding whether the class is canceled or held.

If `at_least == false`, the algorithm will check more elements to see if more than
*"exactly"* `n` elements are found and return `false` in that case.

```cpp title
--8<-- "{sourcedir}/14-angry-prof-again/angry-prof-again-01.cpp:71:71"
```

And this is how we use it in our solution. Nice and easy, exactly as a standard library
algorithm. The complete code is here.

```cpp title
--8<-- "{sourcedir}/14-angry-prof-again/angry-prof-again-01.cpp"
```

## Following The STL Path

<https://cppreference.com> gives us a nice indication as to how `std::all_of`, and family
members, may be implemented and that is by using the `std::find_if` family. The `_of`
algorithms will test if `first` algorithm made it to the `last`, or if it did not to
return the `bool` value that it is expected. For example.

```cpp title="cppreference potential implementation of any_of"
template<class InputIt, class UnaryPred>
constexpr bool any_of(InputIt first, InputIt last, UnaryPred p)
{
    return std::find_if(first, last, p) != last;
}
```

Let us go that route too by creating an `find_n_if` algorithm. This will be the engine
then of our `n_of` algorithm.

```cpp title
--8<-- "{sourcedir}/14-angry-prof-again/angry-prof-again-02.cpp:40:65"
```

It would seem as if we only had to rename `n_of` to `find_n_if` and then get a new `n_of`
written that simply checks if the end of the range has been reached. But how the return
value in `find_n_if` is calculated has changed.

```cpp title
--8<-- "{sourcedir}/14-angry-prof-again/angry-prof-again-02.cpp:55:57"
```

Had we added no optimization checking for the remaining distance to the end for
*RandomAccessIterator* types, we could simply return the last position reached by
`first`. The same happens when it comes to finding exactly `n` elements, because the loop
can be interrupted early. In both cases `first` is never incremented to make it to the
end of the range.

But the return value of `find_n_if` has to be `last` if not enough (or exactly) elements
have been found. Hence the extra check to return the current position of `first` on
success, delivering the position to the caller or actually `last` if some elements could
have still been found but the loop was interrupted early.

Obviously and because the parameters for both functions are the same, the same set of
*SFINAE* constraints apply to both.

Let us get it over with this challenge with a last "complete source code" entry.

```cpp title
--8<-- "{sourcedir}/14-angry-prof-again/angry-prof-again-02.cpp"
```
