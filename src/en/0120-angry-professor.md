# The Angry Professor

**Title**: Angry Professor\
**Link**: <https://www.hackerrank.com/challenges/angry-professor>

In this challenge we can have several test cases with a single input. Each test case has
the total number `n` of students that could attend a class and the minimum number of
attendants, the threshold `k`, when the class is bound to start, for the professor not to
cancel the class. The input has `n` integer values, with negative and zero values
indicating early or on-time arrival. Positive values are used for a late arrival.

And the output uses plain English, *"YES"* and *"NO"** to let us know if the class was
canceled.

Having multiple test cases has one implication: we have to consume the input of a test
case before we can proceed to the next. That implication leads directly to the use an
intermediate storage like `std::vector<int>` and that pushes us in the direction of going
once again for an iterator-based solution rather than showing a plain old approach.

```cpp title
--8<-- "{sourcedir}/12-angry-professor/angry-professor-01.cpp"
```

The code is easy to summarize

  - Create the input and output iterators.

  - Prepare a *lambda expression* to determine an early arrival, i.e.. `x <= 0`.

  - Read the number of test cases, to perform a loop until it is exhausted.

  - Read `n`, `k` (arrival times, threshold) and the `n` elements into a vector.

  - Use `std::count_if` with our *lambda* to count early arrivals

  - Output if the class is canceled by comparing to the threshold `k`.

We should be happy after solving the problem in `18` lines. The code provided
by *HackerRank* simply to manage the input is `101` lines long. Granted, `3` of those
lines are the skeleton for the function the coder has to fill in. Had we used the header
`#include <bits/stdc++.h>` instead of the standard headers, we would have also saved `3`
lines.

## Iterator Attack

The previous solution does not take into account that the threshold, `k`, could be met
without having to check the arrival times of all students. `std::count_if` counts from
the beginning until the end, regardless of what the actual `count` is.

The algorithm will of course check on every iteration if the first iterator, `c.begin()`
is already equal to the last, `c.end()`, and proceed to increase, `++` the first if not.
That is what we are going to take advantage of with our *brilliant* idea: an iterator,
`CountUntilIter` that will be and `end` iterator as soon as the expected count (our
threshold `k`) has been reached. And we will avoid going over the entire range if it is
not needed.

Our iterator takes the container for its construction and the threshold `k`. To make
it flexible it does also take a function that accepts and `int`, our type in the problem,
and returns a `bool`. The container is used to get the `begin` and `end` iterators.

```cpp title
--8<-- "{sourcedir}/12-angry-professor/angry-professor-02.cpp:33:46"
```

The lines above contain the secret sauce. During dereferencing with `*`, the current value
is passed to the early detection function we got in the constructor. The result (`true`
or `false`) is added to a running count and compared to the `until` (the threshold) value
we also got during construction, to determine if the end has been reached and pose as an
iterator having met its end.

During `++` pre-increment, the iterator being moved forward is also checked against the
real end of the range, to also mark itself as ended. This time for real!

And during comparison, to be done with an `end` iterator, the mark we have set it is used
to indicate equality to the end.

```cpp title
--8<-- "{sourcedir}/12-angry-professor/angry-professor-02.cpp:64:68"
```
The iterator is applied with `std::count` the *"if-less"* variant that looks only for a
specific value, `true` in our case. The algorithm does not know that `CountUntilIter`
will perform a sudden jump to its end, but will break out early on that event.

We have also taken the chance to use an `std::array` to hold the cancellation answer,
simply for the sake of it. We did the same with some of the initial challenges, such as
*`Hello, World!`*.

And because *"we can"*, we have also done, as usual, away with using `t`, the number of
test cases, and keep on reading until we hit the *end-of-input*, being therefore capable
of solving any number of cases. If needed be, of course.

Here is the entire code of this second `71` lines solution, still well under the `101`
yet-to-be-solved proposal from *HackerRank*.

```cpp title
--8<-- "{sourcedir}/12-angry-professor/angry-professor-02.cpp"
```

## Rethinking The Iterator Approach

On second thought, we have probably overdone ourselves just a little bit with our
`CountUntilIter` design. The question is we, after all, do really need an iterator or we
need to overcome `std::count` (with and without `if`) algorithm design, to be able to
break out of the counting sequence if a threshold is reached.

Let us therefore design a `count_n_until_k_if` algorithm to rule where all other counting
algorithms fail to deliver.

```cpp title
--8<-- "{sourcedir}/12-angry-professor/angry-professor-03.cpp:7:15"
```

Et voilá! Our algorithm takes a `first` iterator, expected to be an *InputIterator*,
takes also the maximum number of elements to check, `n` and what threshold, `k` to check
for after the current value of the range has gone through the function `fnk`. The `for`
look will interrupted if the running `count` reaches `k`.

The full code is here.

```cpp title
--8<-- "{sourcedir}/12-angry-professor/angry-professor-03.cpp"
```

## Storage-less Counting

At the beginning of this chapter we stated that we had to store the input, because we had
to consume it. But again on second thought, the key is not to store, but to consume if it
is needed.

And here is were *C++17* comes to the rescue again with `if constexpr`.

```cpp title
--8<-- "{sourcedir}/12-angry-professor/angry-professor-04.cpp:37:48"
```

You probably noticed that we were not taking an iterator pair, `first` and `last` and
that was a forward-thinking decision to accommodate an `std::istream_iterator`. With it
our algorithm must no go until the *end-of-range*, because we would be reading past the
next `n` elements and consuming the input and elements of subsequent challenges.

We only have to consume `n` elements. If the `for` loop was interrupted early, the `if
constexpr` checks if the input iterator `first` is of type `std::istream_iterator`,
adding the currently calculated iterator `value_type`. If the check succeeds, as many
elements as needed will be consumed, until the boundary of the next challenge, or the
*end-of-input*, is reached.

In this case we added a complete set of *SFINAE* checks for the iterator and the
value-check function. And we also added a `CASE1` with full storage in an `std::vector`
to check the algorithm under both `if constexpr` results.

(And yet, the code is at `70` lines, well below the *HackerRank* proposal).

```cpp title
--8<-- "{sourcedir}/12-angry-professor/angry-professor-04.cpp"
```
