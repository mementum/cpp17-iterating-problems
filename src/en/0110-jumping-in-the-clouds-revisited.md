# Jumping On The Clouds - Revisited

**Title**: Jumping On The Clouds - Revisited\
**Link**: <https://www.hackerrank.com/challenges/jumping-on-the-clouds-revisited/>

## Skipping The Obvious

Given that we have already gone for the obvious solution in all previous challenges, we
are going to go directly for an iterator-based solution to spare us some lines of text
and code.

The challenge demands that we read a number of `int` values, to be stored in an
`std::vector<int>`, and a constant `k` that will determine the next jump inside the array
of `int` values.

For each jump the initial `e` (*energy value*) of `100` will be decremented by `1`, with
an extra decrement of `2` if the position we landed on contains a `true`, non zero,
value. The jump trip is over when the new position is the initial position.

```cpp title
--8<-- "{sourcedir}/11-jumping-on-the-clouds-ii/jumping-on-the-clouds-ii-01.cpp"
```

As we did previously, we prepare our solution to solve any number of inputs by checking
for *end-if-input* with `std::istream_iterator<int>{}`.

Once the input is gathered the problem is easily solved with a `do-while` loop, instead
of the usual `while` and that is because there is always a first jump. Action that could
actually take us to the beginning. Notice that we only need the `first` iterator, because
the jump calculation formula, `(i + k) % n` ensures that we never get a new position
greater than `n`.

## Mimicking std::istream_iterator<T>

The happy hour idea to improve the solution of this problem comes from looking at the
*STL*. In the problem we use

```cpp title
--8<-- "{sourcedir}/11-jumping-on-the-clouds-ii/jumping-on-the-clouds-ii-01.cpp:24:25"
```

We can quickly make an analogy: we traverse the array (`std::vector<int>`) until we hit
the end of the trip. The fact that the end is the initial position shall not distract us.
We can create an iterator that executes the jumps for us until it comes back to the
original position. Our iterator will simply take the iterator from the `std::vector<int>`
that determines the initial position and jump to the next.

Rather than hardcoding the jump in our iterator, it will accept a function to calculate
the position. The only parameter the function has to take is the actual position.

```cpp title
--8<-- "{sourcedir}/11-jumping-on-the-clouds-ii/jumping-on-the-clouds-ii-02.cpp:8:27"
```

That is our iterator basic definition with member attributes and constructors. The
wrapped-iterator itself is a template parameter, since we do not know in advance what
will come to us.

```cpp title
--8<-- "{sourcedir}/11-jumping-on-the-clouds-ii/jumping-on-the-clouds-ii-02.cpp:30:39"
```

The `*` dereferencing operator will simply return the value of the current position. The
actual job is done in the prefix increment `++` operator, where the current distance to
the start is calculated and the jump is executed using the function `m_fmove` that was
given during construction. To ensure no extra jumps happen, the `m_end` flag is checked
and calculated also here.

```cpp title
--8<-- "{sourcedir}/11-jumping-on-the-clouds-ii/jumping-on-the-clouds-ii-02.cpp:49:58"
```

With such a powerful iterator in the hand we can resort to using `std::accumulate` to
calculate how much energy will be deducted after all jumps have been performed. And once
again the formula is a function parameter, `fenergy`. We do not want to write code in the
solution that can come as a parameter.

We have taken the `typename O` template parameter out of the equation. And this is
because it feels redundant to also pass the `e`, initial energy level, as a parameter.
Our solution function gives us the energy that will be deducted and we can directly
deduct it from the initial energy and output the result, all in the `main` loop.

```cpp title
--8<-- "{sourcedir}/11-jumping-on-the-clouds-ii/jumping-on-the-clouds-ii-02.cpp:70:75"
```
Our `JumpingIterator` is ready and passes all tests. We have marked it as an

`std::random_access_iterator_tag` because it jumps randomly inside the
array range. Granted, it is not a random jump, it is a jump determined by the constant
`k` and the range length `n`, but it fits the description of a *RandomAccessIterator*.

Here is the complete code.

```cpp title
--8<-- "{sourcedir}/11-jumping-on-the-clouds-ii/jumping-on-the-clouds-ii-02.cpp"
```

## Adding SFINAE

Yes, once again we are going to do some dirty work. Someone has to do it! And we do it
because we have, for example, define as parameters the functions that move the iterator
and calculate energy deduction. Hence the need to ensure that they take the right number
and type of arguments and deliver the expected result.

In the case of the iterator the check does not happen in the `struct` definition itself,
but in the constructor. This is the earliest moment at which we can pass the `fmove`
function, and the check has to therefore take place there.

```cpp title
--8<-- "{sourcedir}/11-jumping-on-the-clouds-ii/jumping-on-the-clouds-ii-03.cpp:53:55"
```

The other checks are all made when the templates parameters are defined. We have had to
resort to long strings of beloved `decltype` / `std::declval<T>` married couple checks.
Applying, for example `std::is_integral` to the result of `std::invoke_result_t` to
check if the output type of the `fmove` and `fenergy` parameters is a match would not
work even if it seems optimal.

The full code is presented here.

```cpp title
--8<-- "{sourcedir}/11-jumping-on-the-clouds-ii/jumping-on-the-clouds-ii-03.cpp"
```
