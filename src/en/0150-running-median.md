# Running Median

**Title**: Find the Running Median\
**Link**: <https://www.hackerrank.com/challenges/find-the-running-median/>

## A Set is not a "Set"

The problem gives us first an int. This is the number of values to read. After we read
each value, we have a *"set"* of integer values and we have to print the running median.
I.e,: the value of the int located in the middle (of the sorted *"set"*) for an odd count
of integers and the mean of the two values in the middle if the count is even.

Given that the problem is specifically talking about a *"set"*, let us use an `std::set`
to solve the problem. It is ideal because the *"set"* is always sorted. We will just
insert the value and look for the values in the middle.

```cpp title
--8<-- "{sourcedir}/15-running-median/running-median-01.cpp"
```

Easy, compact, down to 21 lines with no major efforts and it passes our local basic test.

  - We insert the next value

  - We get an iterator to the middle of the `std::set` (already auto-sorted) and the
    value located there

  - If the size even we get another iterator to the next value, add it to our previous
    value and calculate the average

  - Output.

The devil, as always, is in the details. We have a second test with more integer values
that fails to pass.

```
$ make 01 test01 | less
Creating build dir
g++ -std=c++17 -o build/running-median-01 running-median-01.cpp
./build/running-median-01 < running-median.input01 > /tmp/tmp.g7s4f2Zt3o.output
Solution 01: FAILED
---- Expected Output Begin ----
...
---- Expected Output End ----
---- Output Being ----
...
---- Output End ----
---- Diff Begin ----
--- running-median.output01
+++ /tmp/tmp.g7s4f2Zt3o.output
@@ -480,41 +480,36 @@
 51079.5
 51055.0
 51079.5
+51079.5
 51055.0
 ...
```

And the failure has to be given to the use of *"set"* in the problem description. The
input can, and will, have repeated values. What was declared to be a *"set"* is actually
not a set.

## A MultiSet is a "Set"

The standard library is generous and in this case offers us an `std::multiset` that
directly fulfills our requirements. It is a *"set"* that can host multiple repeated keys
and it is still sorted.

```cpp title
--8<-- "{sourcedir}/15-running-median/running-median-02.cpp"
```

We have added 1 line of code to keep a reference to the `itleft` iterator. This is to
optimize the `std::next(s.begin, distance)` calls. We had two of them and we only have
one. The second can be a simple `*(*++itleft)` to get the next position and dereference
it.

This passes the basic local test and the additional `test01` item with repeated values.
But once again this is not the full solution. This code will fail in many test cases with
a *"Time Limit Exceeded"* message.

## The Floating Iterator in the Middle

Even if we have optimized away one of the `std::next` calls the other one is terribly
expensive. It may not seem so, but the iterator of an `std::multiset` (and `std::set`
too) is a *BidirectionalIterator* and cannot be propelled to our required position with
the operator `+=`. It only supports `++` and `--`. This means that `std::next` behind the
scenes has to execute `++` from the beginning for as many times as we want to move the
iterator.

For a local case that is not a problem, but we may be talking of sets hosting millions of
elements.

The solution is clear, we need an iterator that keeps floating in the middle of the set
after we insert a value. Sometimes it will have to move **1** position forward or
backwards after we insert and change the size of the set to be *"odd"* or *"even"*. But
it will be a single `++` or `--` operation.

We are changing the complexity of that bit from `O(n)` to `O(1)`, i.e., from linear to
constant time (and sometimes we will not even move the iterator). The other complexity
factors in the problem remain constant.

```cpp title
--8<-- "{sourcedir}/15-running-median/running-median-03.cpp"
```

Wow! Even adding that logic we have kept the same line count: `22`. Our trick is to use
initially `auto rmed = s.end()` to get the iterator we will float around the middle
point. Choosing `s.end` is just cosmetic to indicate we just need an iterator and not
really the value behind it. At that point in time we could also have gone for
`s.begin()`. With an empty `std::multiset` both point to the same position.

The key is the logic to keep the iterator afloat.

```cpp title
--8<-- "{sourcedir}/15-running-median/running-median-03.cpp:16:16"
```

The one-liner could have been written as:

```cpp title="Unfolded Iterator Logic"
if (val >= rval)
    if (odd)
        rmed = std::next(rmed);
else if (not odd)
        rmed = std::prev(rmed);
```

Translated: if the new inserted val is equal or greater than the midpoint we currently
val (`rval => running val`) and if the insertion has made the size of the container to be
`odd`, move the iterator to the right (forward) one position. Else and if the size if now
`even`, move the iterator one position to the left (backwards).

Yes, we used old tricks such as assignments when a parameter is passed to save a couple
of lines. Example: `s.insert(val = *in++);`. In this case `val` is assigned the value of
the dereferenced iterator and this value is also the parameter for `insert`. The
instantiation of the `for` loop has also gotten a bit more of noise.

## Not All Containers Can Be Floated

It would seem that if we apply the *"Floating Iterator"* technique to another container
we would also succeed. But we will not. See it here applied to an `std::list`, that also
has an *"BidirectionalIterator"*.

```cpp title
--8<-- "{sourcedir}/15-running-median/running-median-04.cpp"
```

The motto here is: *"Find the Difference"*. And the difference is in the application of
`std::find_if` to locate the insertion point. Where our `std::multiset` handled the
insertion in the background, `std::list` needs that we find the insertion point.

The complexity of insertion in `std::multiset` instances is `O(log n)` and not linear,
because the implementation is done with *"red-black trees"* or a similar structure.

In our code above we have tried to reduce the `O(n)` complexity, by only looking for the
insertion point on the half before/after the floating iterator. But that will not change
things by an order of magnitude.

All this means this solution will fail due to the complexity.

## Summary

Let us summarize it here with a well-known saying: *"Complexity killed the cat"*. We had
to move from an initial `std::set` to an `std::multiset` to account for multiple repeated
keys and had to *"invent"* a floating iterator to triumph over the complexity
requirements of the problem.

But as we have proven, the *"Floating Iterator"* is not the panacea that will cure all
evil. An `std::list` with its *Bidirectional Iterator* and linear nature (it is a
**list**) cannot achieve the complexity target.

Let us see what the next chapter will bring to us.

/// pagebreak ///
