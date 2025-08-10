# Variable Sized Arrays

**Title**: Variable Sized Arrays\
**Link**: <https://www.hackerrank.com/challenges/variable-sized-arrays/problem>

## Direct Solution with Vectors

We are being challenged to take a number `n` of arrays and then take `q` pairs of
integers, `i` and `j`, that will be used to search first over the list of `n` arrays with
`i` and then the element `j` in the referenced array.

Let us spare us using dynamically allocated arrays with `new[]` and then deallocating them with `delete[]` before ending the program (or having used an `std::unique_ptr`) and go directly for a manual solution featuring `std::vector`.

```cpp title
--8<-- "{sourcedir}/10-variable-sized-arrays/variable-sized-arrays-01.cpp"
```

As we already know we need a lot of `std::cin >> varname` boilerplate after having
defined the variables. The only novelty here is that we use `std::vector::emplace`, to
let the inner vectors be constructed for us.

```cpp title
--8<-- "{sourcedir}/10-variable-sized-arrays/variable-sized-arrays-01.cpp:14:14"
```

Since we get an iterator to where the new `std::vector<int>` has been positioned, we keep
a reference and using `std::vector::push_back` is straightforward.

To get the code in as few lines as possible, we use the "dirty" trick of defining the
input variables in the `for` loop and then performing the storage or output operations in
the final statement of the loop.

## Iterator-Based Solution

Converting to an iterator based solution is easy. Even better, the "real" solution takes
less lines if we take into account the two extra `#include` files we need. We do, of
course, apply the same "dirty" `for` trick to save some lines.

```cpp title
--8<-- "{sourcedir}/10-variable-sized-arrays/variable-sized-arrays-02.cpp:14:14"
```

Because the vector is being created for us, we can use the reference we keep to
`std::copy_n` the `n` elements for each instance with `std::back_inserter`. We already
used in a previous challenge in which we also worked with arrays. Let us remark the fact,
that by using it we are in fact also using an iterator. It is created in the background
for us and takes advantage of the `std::vector::push_back` method.

## Prototyping A Container

Given that we have a container, `std::vector`, holding another container, also an
`std::vector`, it seems like natural that we try to prototype a container to do the work.

To save us some work later during the refinement phase, let us already add some *SFINAE*
checks. To do so we are going to resort to one of the oldest technologies in `C++`
(coming from the `C` times), the preprocessor. Yes, that macro machinery that evoke the
80s.

```cpp title
--8<-- "{sourcedir}/10-variable-sized-arrays/variable-sized-arrays-03.cpp:8:30"
```

Looking at that code the reason is obvious: *"DRY"* (*Don't Repeat Yourself*). With the
`DEFINE_HAS_METHOD` and `DEFINE_HAS_METHOD_ARG` we avoid almost endlessly repeating the
same boilerplate to look for methods in classes. Although not shown above, we also have a
`has_insert_v` checker. This one looks for a method with two arguments and the macro
would have taken exactly the same place as the only use we make of it.

All those will be combined in a two extra checks to see if the template parameters for
our container prototype are supported.

```cpp title
--8<-- "{sourcedir}/10-variable-sized-arrays/variable-sized-arrays-03.cpp:45:66"
```

`VariableSizedArrays` will also use the checks in combination with `if constexpr` to
select the appropriate methods at compile time, to support our solution. For example, a
`get_inserter` method will choose if `std::back_inserter` is the one to use or
`std::front_inserter` is the one to go for.

```cpp title
--8<-- "{sourcedir}/10-variable-sized-arrays/variable-sized-arrays-03.cpp:80:90"
```

This method works for the selection of the outer array and the inner arrays, the ones
holding the values. And because we have used different methods for the insertion and that
means a different positioning, we also need to use `if constexpr` to select the method
taking us to a given position, the *query*, in the outer and inner arrays.

```cpp title
--8<-- "{sourcedir}/10-variable-sized-arrays/variable-sized-arrays-03.cpp:70:78"
```

Both methods are used to implement the `read_array` and `query` methods, the ones doing
the work for the outside world, i.e., for `main`.

```cpp title
--8<-- "{sourcedir}/10-variable-sized-arrays/variable-sized-arrays-03.cpp:93:104"
```

The final solution offers a `CASE1` option (build and test with `make 03 case1`) that
uses `std::deque` for both the inner and outer arrays, selecting `std::deque::push_front`
and `std::front_inserter` for the implementation of the private methods supporting the
external interface.

```cpp title
--8<-- "{sourcedir}/10-variable-sized-arrays/variable-sized-arrays-03.cpp"
```

## An (almost) real container

The container prototype we crafted above does the job, but it feels awkward. It has no
`begin`, no `end` and no `push_xxx` methods and that means we have to resort to handmade
`read_array` and `query` methods.

Let us therefore implement those *container* methods to have an (almost) real container.
`begin` and `end` will have to return an iterator and that means we have to also code one
inside the container class.

And *Houston* ... *we have a problem*. Iterators are supposed to work linearly and the
distance from one point to another must be linear because it has to be integer-like. It
is in the standard. Nevertheless, we can work around the *"limitations"* by tagging our
iterator as n *RandomAccessIterator*.

In the case of other iterator types, moving the iterator forward or backwards is done in
this fashion. Going forward, for example:

```cpp
   --distance;
   ++iterator;
```

When distance reaches `0`, the iterator is no longer `++`ed. The iterator can only
operated inside the increment operator without actually knowing what the value of
`distance` is.

But for a *RandomAccessIterator* the movement works like this:

```cpp
   random_it += distance
```

The iterator itself has to move using the `distance`. That is exactly what we are going
to *"abuse"* by creating a *2-D* distance, that will always take us from the beginning of
the container to the position the query wants resolved.

Here is our Distance/Position implementation

```cpp title
--8<-- "{sourcedir}/10-variable-sized-arrays/variable-sized-arrays-04.cpp:66:82"
```

Ideally we would have used an `std::pair<int, int>` directly, but reality killed the cat.
The library implementation used by *GCC* relies on the fact that the distance for
iterators has to be integer-like and makes a comparison with `1` and `-1`. Hence the need
to wrap our `std:pair<int, int>` to provide the proper comparison operator and avoid
chaos during compilation.

That distance definition will be used in the `operator +=` to move to the desired
position. The implementation does not consider the current position: it does simply take
the given position as the new point the iterator has moved to. That position will later
be used when the iterator is dereferenced with the operator `*`.

```cpp title
--8<-- "{sourcedir}/10-variable-sized-arrays/variable-sized-arrays-04.cpp:99:113"
```

As it can be seen, the dereferencing uses a real iterator passed during construction,
`m_it` to find first the inner container and then move into the final position within it.

Our container chooses if the iterator from which the position is calculated, will be
`begin` or `rbegin` from the outer array container.

```cpp title
--8<-- "{sourcedir}/10-variable-sized-arrays/variable-sized-arrays-04.cpp:135:148"
```

To complete the container, we reuse the previous `get_inserter` to implement the
`push_back` method.

```cpp title
--8<-- "{sourcedir}/10-variable-sized-arrays/variable-sized-arrays-04.cpp:155:161"
```

With the container in place, we can now use the standard library tools to store the input
arrays and to execute the queries. Creating an array with the standard template
parameters is now a matter of directly applying `std::copy_n`.

```cpp title
--8<-- "{sourcedir}/10-variable-sized-arrays/variable-sized-arrays-04.cpp:173:177"
```

And executing the query a matter of moving the `begin` iterator with a *2-D* distance,
i.e., a pair of integers to the required destination and dereferencing the resulting
iterator.

```cpp title
--8<-- "{sourcedir}/10-variable-sized-arrays/variable-sized-arrays-04.cpp:191:193"
```

## Summary

As in previous examples we have iterated from a straightforward solution to an iterator
based solution and we have moved a step further by implementing a container that does the
iteration for us. The container uses a *2-D* distance to reach the destination desired by
the query. We have even removed the limitation on the number of queries by testing for
the end of input before we stop reading values.

Here is the complete code.

```cpp title
--8<-- "{sourcedir}/10-variable-sized-arrays/variable-sized-arrays-04.cpp"
```

/// pagebreak ///
