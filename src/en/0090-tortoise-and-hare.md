# The Tortoise And The Hare

**Title**: Bit Array\
**Link**: <https://www.hackerrank.com/challenges/bitset-1/problem>

The problem description would seem to indicate that one will have to scan an array of `N`
elements and length *10^8^*. But considering that the operation that fills the array
restricts the integers to be *modulo 2^31^*, there will be a sequence repeating itself.
But if the length of the array is restricted, one may hit the end before the sequence
starts repeating.

Therefore, finding the number of different integers boils down to detecting a cycle. The
[*"Floyd's Tortoise and Hare"*][:^wiki-cycle] algorithm is certainly a good
choice.

[:^wiki-cycle]: Wikipedia: https://en.wikipedia.org/wiki/Cycle_detection

```cpp title
--8<-- "{sourcedir}/09-tortoise-and-hare/tortoise-and-hare-01.cpp"
```

A single movement for the *Tortoise* is calculated with the *lambda* assigned to `fmove`.
The *Hare* uses `fmove` twice in each round.

Notice that we have to check if the array has been traversed before moving our heroes.
Consider the simple case where `N = 1`. The solution will obviously be `1`, and there is
no cycle to detect, although philosophically, one could say we are facing an eternal
cycle.

## Iterating the Range

Looking at the problem we can identify two sets of movements running in parallel, and
each of those sets can lead to the solution.

  1. Moving along the range `1 => N`, the maximum array length, and checking if `N` is
     reached

  2. Moving the *Tortoise* and the *Hare* and checking if they occupy the same array
     position before `N` is reached. In this case the distance traveled between `1` and
     `N` is the solution.

To address the first set of movements we will be using our `Range` virtual container, the
one emulating Python's `range` that we developed for the ["For Loop"](0070-for-loop.md#0070-for-loop)
problem.

The second one has been addressed with our *lambda*. However, the check in the `if`
expression to see if the cycle has been detected will look smarter and more elegant by
abusing the *lambda* expressions and packing the check in a single function.

Given that we have *find* a cycle, or the lack of it, the standard `std::find_if` seems
perfectly suited for the task. If the cycle is found, with the unary predicate we feed it
with, we will know at which position. Else it will run until the length of the array is
reached. This will also come as a natural result of using `std::find_if`, because the
returned iterator will be at position `N`, right after the end of the array, that is a
half-open interval `[1, N)`. Here are the *lambda* expressions.

```cpp title
--8<-- "{sourcedir}/09-tortoise-and-hare/tortoise-and-hare-02.cpp:81:89"
```

Notice how our `ftoha` cycle-detecting *lambda* defines a `const auto &` argument for
which we provide no name, as we simply ignore it. It is the position in the array.
Position that is only relevant for the answer and not for the cycle detection.

We have, of course, also resorted to input/output iterators to fetch the input parameters
and to create a custom *STL-like* solution function. This can already take a function
returning `bool` and taking a parameter matching the type of what dereferencing the
iterator would give us.

```cpp title
--8<-- "{sourcedir}/09-tortoise-and-hare/tortoise-and-hare-02.cpp:60:71"
```

*SFINAE* here would help us with a check using `std::enable_if` that returns
`Predicate<I>`, to give us the same type that we have manually defined, but after having
checked the iterators. We will tackle that on our next iteration to solve the problem.

Here is this entire solution.

```cpp title
--8<-- "{sourcedir}/09-tortoise-and-hare/tortoise-and-hare-02.cpp"
```

## Real Iterators

We have not really used proper iterators in the previous solution and this is something
we can address by defining our custom iterators.

Recall that `std::istream_iterator<T>(std::istream &)` has a counterpart to mark the end
of the iteration. This can be invoked as: `std::istream_iterator<T>()`. We are going to
implement that concept with an iterator named: `CycleIterator`. As the name seems to
indicate it will be in charge of finding if there is a cycle or get to the end of the
defined range.

Since we want to *"find"* a cycle, using `std::find_if` seems semantically the best *STL*
verb to use from the available algorithms. Recall that `std::find_if` needs a
*bool-returning* predicate to indicate, in our case, that the cycle has been found and we
get the dereferenced value of the current iterator position. The cycle detection cannot
be done merely by looking at the current position, but that is, for example, what we have
to obtain by dereferencing the iterator if the end of the range is hit.

Houston, we need a dereferenced value that holds two values simultaneously. Nothing that
cannot be solved with an `std::pair<T, T2>`. But let us be brave and use instead the more
general `std::tuple` with just two types. We already used `std::pair` when using an
`std::map` and now we will be able to use `std::get<N>(x)` to get the needed values.

```cpp title
--8<-- "{sourcedir}/09-tortoise-and-hare/tortoise-and-hare-03.cpp:8:17"
```

There is the definition of our tuple and how this is the value used in the iterator.

We will also need to hold the current position in the range, the positions of the
*tortoise* and the *hare*, and the current combo "cycle detection + position" inside the
`std::tuple<bool, int>`.

```cpp title
--8<-- "{sourcedir}/09-tortoise-and-hare/tortoise-and-hare-03.cpp:19:27"
```

Calculations are done in our `operator ++()`. Notice that we have commented out the `->`
operator and the postfix increment. They are not really needed.

```cpp title
--8<-- "{sourcedir}/09-tortoise-and-hare/tortoise-and-hare-03.cpp:40:50"
```

Notice how we have customized how many times the *tortoise* and the *hare* can move. The
default values in the constructor are `1` and `2`, but nothing prevents us from using
other speeds.

The constructor is something we need to look into.

```cpp title
--8<-- "{sourcedir}/09-tortoise-and-hare/tortoise-and-hare-03.cpp:29:38"
```

First, our *"end-of-range"* constructor at the end of the snippet. It takes the end
position of the range, `N`, and is a no-op for all things.

Then our regular constructor that takes the function that moves the *tortoise* and the
*hare* and allows customizing where to start and how to move. Something important to
notice:

  - The `start` position is taken with a `-1`

  - The prefix increment `++` is called during initialization. This will effectively
  restore `start` (adding `1`) and will calculate the initial first-jump positions of our
  competitors. If the first comparison happens to detect the cycle, the result will be
  `start` that we have restored to its original value.

Our solution function needs some explaining for sure.

```cpp title
--8<-- "{sourcedir}/09-tortoise-and-hare/tortoise-and-hare-03.cpp:56:63"
```

We define a predicate `pred` that will get and return the first value of whatever has
been dereferenced. We know it is a `bool` in the `std::tuple<bool, int>`.

The final output over `out` is the second value, the `int`, be it because a cycle has
been detected or because the end of the range has been reached.

Unfortunately, we cannot let `pred` be a parameter, because it needs internal knowledge
of the structure holding the cycle detection and the range position.

The `main` function has now been cleaned of all *lambda expressions* but `fmove`, that
goes into the `CycleIterator` to push the range of the *tortoise* and the *hare*.

See the entire code, including the `main` function below.

```cpp title
--8<-- "{sourcedir}/09-tortoise-and-hare/tortoise-and-hare-03.cpp"
```

## Real-Real Iterators

There is a fundamental problem in the design shown above because we place `N`, the end of
the range, in the `end-of-range` iterator. That means that we departed from the
`std::istream_iterator<T>()` design to mark the end of the range. It seemed natural to
put that `N` there as a kind of `EOF` marker. We will work on changing that.

However, the most fundamental flaw is the use of `std::tuple<bool, int>` as the magical
tool to sometimes deliver the indication that the cycle has been detected and where (or
if the end of the range has been reached). Although it seems ideal, we were actually
forced to use such a construction because we let ourselves be misled by the semantics. In
order to *"find"* the cycle we deemed `std::find_if` as the right tool.

Breaking out of the search for the cycle can only be done if the iterator itself signals
the cycle has been found. It seems therefore redundant that we need a *lambda* accessing
the first value inside our `std::tuple<bool, int>` and blindly returning it. The cycle
has already been detected and the iteration could stop *"on its own"*, i.e., by meeting
the requirement of being equal to the `last` iterator.

Going a couple of steps backwards and looking at the original problem definition, what we
want is to *"count"* the number of different integers before those integers start
repeating. Let us therefore switch to using `std::count`.

```cpp title
--8<-- "{sourcedir}/09-tortoise-and-hare/tortoise-and-hare-04.cpp:60:68"
```

We will be counting as long as the iterator returns `true`. As a bonus we also have
another implementation using `std::accumulate`, that will start with an `init` value of
`0` and add all the `true` instances, a value that obviously is converted to `1`.

Correspondingly, our iterator will simply return `true` every time it is dereferenced. As
simple as this.

```cpp title
--8<-- "{sourcedir}/09-tortoise-and-hare/tortoise-and-hare-04.cpp:40:40"
```

To do so we remove the `std::tuple<bool, int>` value holder and add a boolean marker for
the end.

```cpp title
--8<-- "{sourcedir}/09-tortoise-and-hare/tortoise-and-hare-04.cpp:18:38"
```

We have also changed the constructors, with the regular one taking the beginning and end
of range and the *"end-of-range"* taking no parameters. The construction in `main`
therefore changes. To be noted: the regular constructor no longer increments the iterator
during initialization. But it is able to detect if the range is of zero length because
`N` is `1`, i.e., the iteration has to immediately stop.

```cpp title
--8<-- "{sourcedir}/09-tortoise-and-hare/tortoise-and-hare-04.cpp:81:85"
```

Once the iterator detects the cycle it will mark itself as ended, making it *"equal"* to
the *"end-of-range"* iterator. The detection and marking do happen inside the prefix
increment operator.

```cpp title
--8<-- "{sourcedir}/09-tortoise-and-hare/tortoise-and-hare-04.cpp:43:50"
```

To make sure that the iteration is stopped by the internal loops inside `std::count`,
`std::accumulate` or a manual loop we could code ourselves, the `operator ==` has to be
also tweaked to use the `m_end` marker and compare itself to the *"end-of-range"*
iterator.

```cpp title
--8<-- "{sourcedir}/09-tortoise-and-hare/tortoise-and-hare-04.cpp:54:56"
```

With all the pieces in place, our approach solves the problem neatly with either
`std::count` or `std::accumulate` (compile and test with `make 04 case1` to use the
latter).

Should you implement your own solution, there are two test cases available. This is so
because the sample test provided in the problem description is an *end-of-range* test
case. We also need to verify we do not make it to the end if a cycle is found.

Testing with the standard test case requires a simple make.

```
make 04
```

To test with the cycle-exists test case add a `test1` to the make clause.

```
make 04 test1
```

## Summary

We finally have an iterator-based solution that is also customizable with custom ranges
to traverse, start position for the *tortoise* and *hare* markers and additionally
accepting a custom function to move the markers. Mission accomplished.

/// note
*SFINAE* has been left out in this occasion to keep things shorter and because we have
already done all the tests in previous solutions.

The reader may have also noticed that `main` can now take several problems by reading
problem inputs until the *end-of-input* is reached. This is not part of the problem
description but it seemed like a nice final touch.
///

---

Here is the code for the last, final, and great solution.

```cpp title
--8<-- "{sourcedir}/09-tortoise-and-hare/tortoise-and-hare-04.cpp"
```

/// pagebreak ///
