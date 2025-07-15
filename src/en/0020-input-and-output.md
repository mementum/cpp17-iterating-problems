# Input and Output

**Title**: Input and Output\
**Link**: <https://www.hackerrank.com/challenges/cpp-input-and-output>

After the classic, output-only *"Hello, World!"*, we are confronted with first accepting input, performing then a simple manipulation of the data and sending finally the result to standard output. I.e.: read three integers from standard input, add them up and output the result.

The straightforward simplistic solution.

```cpp title
--8<-- "{sourcedir}/02-input-and-output/input-and-output-01.cpp"
```

Not really the challenge we were looking for. Let us immediately go for a first iterator based version, using also `auto`.

```cpp title
--8<-- "{sourcedir}/02-input-and-output/input-and-output-02.cpp"
```

Unlike in the *"Hello, World!"* case, we have real input and that is why we introduce an iterator to gather the input: `std::istream_iterator`. Just like its counterpart the `std::ostream_iterator`, it takes a stream (an input one, obviously) and reads characters that will be converted to the type used in the template. Whitespace, including newlines, and End-of-file are used as the separators to discriminate the input values.

The solution is not really satisfying, even if we can use iterators and apply `auto` to all variable declarations. It is still hardcoded to take three `int` values. Notice that we do not need to even store the values, thanks to the use of the input iterator.

Moving forward is straightforward: if there is an input iterator, there must, for sure, be a counterpart marking the end of the stream. Recall that our input range in the previous solution had `begin`/`end` member functions precisely for that. Using here an *end-of-stream* iterator we could read integers until the input is exhausted, making the solution dynamic.

Such *end-of-stream* input iterator is instantiated by not passing a stream to the constructor, i.e.: `std::istream_iterator<int>();`.

At the same time, we can use the proper algorithm from the *STL* to add up all integers: `std::accumulate`. To spare us some boredom and as we did with the *"Hello, World!"* problem, we will move the core of our solution, even if only a wrapper, directly to a generic function.

```cpp title
--8<-- "{sourcedir}/02-input-and-output/input-and-output-03.cpp"
```

For the sake of it, we may even go for a single statement solution inside of `main`.

```cpp title
--8<-- "{sourcedir}/02-input-and-output/input-and-output-04.cpp"
```

This is a lot more like it. If you look at how this is usually solved, the problem solver will first copy the input, to an `std::vector`, which will then be used as the input range for `std::accumulate`. I.e.: an input range is used to create another input range, that is then used for the solution. It really seems we are better off by directly using the available input range.

## Checking the Nature of the Iterators

Stopping here would go against the goal of our quest, i.e.: learning, learning more, learning even more, be better and write better code. Let us therefore carry on by adding some checking for our template parameters with *SFINAE*.

When I first heard about *SFINAE* I had to make sure I was getting it right: *"Substitution Failure Is Not An Error"*. Because once I managed to wrap my head around it, I thought that *SFIAFWAD* would have been a better acronym: *"Substitution Failure Is A Feature Working As Designed"*. No matter in which industry you have worked, it is quite often that an error is deemed to be a feature and tagged with the label, "it works as designed". It is anyhow highly unlikely that my proposal would have probably made the cut for the naming.

Let us see why we would have to use *SFINAE*. In our two solutions, we have modeled our functions in this manner.

```cpp title="Typical Solution Model"
template<typename I, template typename O>
auto
solution_function(I first, I last, O out) {
    std::do_something(first, last, out);
    // or *out++ = std::do_something(first, last);
}
```

Our *STL*-like `solution_function` accepts the (until compilation) unknown types `I` and `O` and pass them completely unchecked to `std::do_something`, or feeds the result of the latter to the output iterator.

Inside the function, it is assumed that `I` is an input iterator and `O` is an output iterator, but we have no certainty if those assumptions are actually true. Compilation will most probably fail at some point if they are not, but it may also go through causing a spectacular runtime.

We could do the following and our on-the-fly checking IDE will not complain: `solution_function(5, 6, out);`. Those two integers are, obviously, no iterators and it would be nice to catch the error as soon as possible, ideally whilst typing.

Here is where *SFINAE* comes to the rescue. By imposing some restrictions and having the parameters checked during compilation or by our IDE, the iterators will be checked to see if they are really what they claim to be. *SFINAE* works by removing potential template candidates. There will be no match for our call if the arguments do no fit our restrictions.

Yes, *C++20* introduces `concepts` but we are still using *C++17*, so we have to settle for *SFINAE*, the next best thing after sliced bread. Our template-based `solution_function` will be enabled if some conditions are met. To implement that *enabling* (or *disabling*) we will be using `std::enable_if` as the basis. This utility function will fail if the boolean conditions passed to it are not met. But if the conditions are met, we will be able to use the type passed to it as the result. The general form is: `std::enable_if<condition, enabler>`, where in this case the *enabler* will be used as the return type of our `solution_function` algorithm.

To use the `enabler` as a type, we have two options:

  - `std::enable_if<condition, enabler>::type` (*C++11*)

  - `std::enable_if_t<condition, enabler>` (*C++14*)

We will go for the latter because it seems cleaner, `_t` for `::type`, and because *C++17* introduced more of these helpers as we will see below, i.e.: others did also see the benefit in using the `_x` suffix as opposed to `::whatever_we_want`.

The most obvious idea is checking if the parameters bear the proper iterator tags for their intended functionality. Let us add some vertical spacing here or the sake of clarity. We are going directly for the helper with `_v` for `::value` when using `std::is_same`.

```cpp title="SFINAE - Checking if something *is exactly* something"
template <typename I, typename O>
std::enable_if_t<
    // enable if condition
    std::is_same_v<
        std::input_iterator_tag,
        typename I::iterator_category>
    &&
    std::is_same_v<
        std::output_iterator_tag,
        typename O::iterator_category>
    ,
    void
    > // get the type of the enable_if result
solution_function(I first, I last, O out) {
    std::do_something(first, last, out);
}
```

`std::is_same_v`, a *C++17* present, will be substituted with `true` or `false` during compilation. If the logical `and` test, of those conditions, has a single `false`, `std::enable_if_t` will fail and the candidate will be removed from the set of possible matches for an invocation of `solution_function`.

This unfortunately has some problems.

  - We are checking with `std::is_same_v<T>` (aka `std::is_same<T>::value`) and our iterator could be not only an input iterator, but actually a lot more. A *RandomAccessIterator* is also an *InputIterator* that can do a lot more things.

  - We check directly `I::iterator_category` and `I` (or `O`) could be a simple type like a `const char **`, that the *STL* can manage as an iterator. Compilation failure knocking on the door in 3, 2, 1.

Luckily for us, both issues have a solution.

  - Run the check with `std::is_base_of`. The function will check inside `std::input_iterator_tag` to see if it is the base class of whatever tag the iterator has declared in its implementation. This will then be a positive check not only for an *InputIterator* but for others too, such as the aforementioned *RandomAccessIterator*.

  - Use `std::iterator_traits<T>` to look for the tag. This *STL* functionality understands what is and what can be an iterator and offers a unified interface to retrieve the actual declared `iterator_category` tag or the implied one in case of thingies such as `const char **`.

Let us apply all this to our `input_and_output` challenge. Using the `_v` variant of `std::is_base_of`, of course.

```cpp title
--8<-- "{sourcedir}/02-input-and-output/input-and-output-05.cpp"
```

There are two small improvements that still can make the cut.

Let us first have a look at how `std::enable_if` is defined.

```cpp
template< bool B, class T = void >
struct enable_if;
```

Great for us. We could skip the second template parameter because it is already `void`, exactly our return type. Improvement number one is already there.

The second change deals with how we have defined the extra parameter `init` for our `input_and_output` solution.

```cpp
input_and_output(I first, I last, O out, int init = 0) {
```

This could break if we were using a different type for the input and output, like a custom class that had no conversion to/from `int`. Let us use the template facilities just a bit more. The final code is here.

```cpp title
--8<-- "{sourcedir}/02-input-and-output/input-and-output-06.cpp"
```

For the sake of a more general approach we even added an alias for `int`, the type used in our solution. All template instantiations use now `prob_type` and with a single change to `using prob_type = int;`, we would change the type everywhere.

But this feels so unnatural. Right before this last piece of code we were free to call `input_output` without any template parameters, because the instantiation was straightforward. We now need to specify the three of them. Unless we always specify a value for `init`, to let the template facilities deduct the type, as it happens in the following code.

```cpp title
--8<-- "{sourcedir}/02-input-and-output/input-and-output-07.cpp:21"
```

The problems should be obvious even for the untrained eye (i.e.: the author's)

  - First, we are forced now to always provide an argument that we have conceived as having a default value.

  - Second, we could pass something else instead of `0` with an incompatible type. So much for our idea to make `init` as generic and default as possible. Or maybe not.

## Summary

Let us cut our losses short, because we have achieved a lot. Having real input in the hand, we have removed the fixed constraints of the challenge and can read any number of values, not just three. Moreover, our *STL*-like solution has gained type checking for the parameter templates.

But, but, but ... "Houston, we have a problem", because our extra parameter is causing problems and the reader has for sure noticed that we have lost our `auto`-magic return type in the solution. The type is now manually determined, when we use `std::enable_if`. Luckily, nothing lasts forever as we will see.
