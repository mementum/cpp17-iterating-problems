# The Angry Professor - Reloaded

**Title**: Angry Professor\
**Link**: <https://www.hackerrank.com/challenges/angry-professor>

During the previous chapter there were two *"on second thought"* moments. Let us keep on
second guessing ourselves by looking for alternatives.

## Poor Man's Sorting Approach

Whilst thinking about how the problem could be solved by looking at the standard library,
the idea of sorting the `std::vector<int>` was an option. But sorting the array and then
looking for the boundary where arrival times turn to positive seemed like a very
expensive idea.

And this is where `std::partition` enters the game arena. It offers us the possibility to
sort, but without sorting. It delivers two *"partitions"* (hence the name), where the
guarantee is that the elements on the first half meet a logical condition we provide and
the elements on the other half do not.

Even better, the return value tells us, with an iterator, exactly where the boundary is,
i.e., the *"pivot"* point. Given that we already have a range and a logical condition
too, `x <= 0` there is no harm in trying.

```cpp title
--8<-- "{sourcedir}/13-angry-prof-reloaded/angry-prof-reloaded-01.cpp:8:13"
```

As easy as that. We partition the container, notice we take a non-const reference to it,
using a logical condition. The same as in all cases in the previous chapter. And having
the *"pivot"* point as the return value, the number of early students is simply the
`std::distance` from the beginning to the *"pivot"* point.

This is probably less than ideal. We modify the container and this may not be a desired
side effect in real challenges. Not taking a non-const reference could, and would, be
probably worse because we would be making a copy of the container, making the operation
really expensive in terms of time and memory.

The full code for this solution.

```cpp title
--8<-- "{sourcedir}/13-angry-prof-reloaded/angry-prof-reloaded-01.cpp"
```

## Filtering The Input During Insertion

The thought of modifying the container brings us another idea: modify the container right
at the beginning, before it is even in the container by filtering out the late arrival
times.

We just said that modifying the container in-place or copying it were bad ideas, but what
about if we only care about the content meeting our logical condition. Those
*"early-students"* may not only make the class possible, it may logical to think they
might be rewarded somehow at some point in time.

Our `std::vector<int>` building approach gives us the perfect entry point to implement
this idea. Because we do not copy into a container built with a fixed number of empty
elements, we do **"insert"** with `std::back_inserter(c)`. Let us replace that *"inserter"*
with our custom version, returning our custom output iterator instead of what the
standard library gives us.

We only need to rely on the container providing a `push_back` method for insertion (hence
the name `back_inserter`). The initial function called is just a helper and we create our
own helper now.

```cpp title
--8<-- "{sourcedir}/13-angry-prof-reloaded/angry-prof-reloaded-02.cpp:38:42"
```

Unlike the `std::back_inserter` and to support flexible filtering, we take a second
argument: the function that takes the element to be inserted and returns `true` or
`false`, or something convertible to a `bool`.

We already did an *OutputIterator* with our *"Hello, World!"* challenge, because we
needed the *", "* string as the separator, but first starting with the second element.
Building on that experience, we know that only the `operator =` method is operational and
all other are `no-op`. Here is our operational method.

```cpp title
--8<-- "{sourcedir}/13-angry-prof-reloaded/angry-prof-reloaded-02.cpp:29:36"
```

It is not rocket science, but not far from it. The value, of the type contained in the
container is given to the custom logical function taken during construction. If the check
passed, the value is inserted. If not, so be it and we let value continue its travel into
the unknown.

This, of course, simplifies how we approach the problem in the main loop, as there is no
longer a call to any solution function, because the problem is automatically solved by
the `size` of the constructed container.

```cpp title
--8<-- "{sourcedir}/13-angry-prof-reloaded/angry-prof-reloaded-02.cpp:57:58"
```

Done and dusted! The only criticism here is that we lose some information by not string
the late arrival times. Whether we would need them is another story.

Let us present the code of this solution.

```cpp title
--8<-- "{sourcedir}/13-angry-prof-reloaded/angry-prof-reloaded-02.cpp"
```

## Bonus SFINAE-Enabled Version

The same version but with the *SFINAE* checks in place to make sure we have something
container-like for the insertion and the proper logical check function. Simply because we
can. And because we can, we have reused the *preprocessor* macros to check for the
presence of a method, to look for `push_back`, `push_front` and `insert`. Rather than
having an *"inserter"* that is hardcoded to only push at the back, we can support
containers that insert using different methods.

Notice that the resulting `has_method_v` construct does also check for the presence of
`::value_type`, because if it is not there, `std::void_t` will fail to deliver and the
*SFINAE* check will fail. That is all we need to check to believe that the type is a
container, even it is faking it.

We use `::value_type` in our `operator =` implementation to keep it generic and to see if
the logical check function can be invoked with that value type. This is where we will
again apply `if constexpr` magic to choose the proper insertion method.

And because we are no longer bound to `push_back` we have changed the names of our helper
and iterator marvels, to remove `back` from the name and use a proper `filter` name
prefix for them.

Before presenting the new monster code, let us double down on the small *number-of-lines*
joke. Recall that *HackerRank* proposes `101` lines just to gather the input, including
`3` for the skeleton of the solution. In all previous instances we were always well under
that line count. This time we have `118` and that means that for all instances and
purposes we have overtaken the *HackerRank* proposal. And that even if we consider
solving the problem, because a trivial solution with the `std::vector<int>` in the hand
can be compressed into a one liner.

Let us go for the code.

```cpp title
--8<-- "{sourcedir}/13-angry-prof-reloaded/angry-prof-reloaded-03.cpp"
```
