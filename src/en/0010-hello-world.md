# Hello World

**Title**: Hello, World!\
**Link**: $[https://www.hackerrank.com/challenges/cpp-hello-world]$ 

Getting started with any programming language always implies a "Hello World" example and Hackerrank also has this challenge to get problem solvers started with *C++*. Solving the problem is obvious, but the goal was not to make obvious things. Rethinking the problem to make something useful out of it is the real challenge. Let us see how to rework this into something interesting.

We first draft the obvious solution, considering that the expected output is: *"Hello, World!"*.

```cpp title
--8<-- "{sourcedir}/01-hello-world/hello-world-01.cpp"
```

There it is. The solution does not even require a `\n` (or `std::endl`) after printing the message. But notice that a comma, followed by a space, is used as a delimiter unlike in many other cases. That will be our first challenge.

## Using an Output Iterator

Because the goal is to solve the problems, where possible, with a Standard Library approach, the solution above is not really satisfying. Let us therefore introduce a classic from the *STL*: iterators. We will be adding an output iterator to the mix, in this case: `std::ostream_iterator`, that will take over the functionality of `std::cout`.

```cpp title
--8<-- "{sourcedir}/01-hello-world/hello-world-02.cpp"
```

We have added our output iterator, initially managing it manually with the `*` (dereference) and `++` (postfix increment) operators to output our string for us. This still bears no resemblance to an *STL* solution. Luckily, our `std::ostream_iterator` can take a second parameter during construction, which is the delimiter to be used after each output operation. We can use it to get closer to the final goal.

Notice that we have used `std::string` with `std::ostream_iterator`. We could have used a good old `const char *` but our focus is the Standard Library.

## Adding Input Iterators

Let us now simulate that there is also input, by managing *"Hello"* and *"World!"* as separate tokens and making the `, ` delimiter a part of the output operation and not part of the content itself.

```cpp title
--8<-- "{sourcedir}/01-hello-world/hello-world-03.cpp"
```

This is much better. By using an `std::array` to hold the two words in our message, we can use the `begin` and `end` methods to iterate over the input range and then output each element with our iterator. Even for this simple case, we obviously avoid checking against `end()` each and every time. Imagine if we did that for an array of millions of elements, instead of storing the `last` iterator in a variable.

We are using the *C++17* `std::array` facilities, with which it is possible to forego the template syntax, avoiding the need to specify the type of the array elements and the array size. The compiler will do it for us. And this is yet another reason `std::string` was a good choice as the type for `std::ostream_iterator`. We could quickly find out if the compiler is choosing `const char *`, however by choosing `std::string` there is no need to worry, because it will be converted to a `std::string` when moving it to the output.

In any case this solution fails to pass the test because the output has a trailing comma.

```
Hello, World!,
```

An improvement is still possible, by using a range based `for` loop, instead of manually controlling the iteration.

```cpp title
--8<-- "{sourcedir}/01-hello-world/hello-world-04.cpp"
```

Some syntactic sugar that simplifies the look of our `for` loop and removes the direct use of the input iterators from the equation. The change implies no change for the result: still a failure due to the aforementioned trailing comma.

## Adding an STL Algorithm

One of the goals was to use the *STL* as much as possible. And there is an ideal target for us: `std::copy`, which takes an input range and copies each element to a destination range, by taking an iterator that points to the first element of the output range. This destination iterator has to be at least an output iterator. We already have those elements at hand: an input range and an output iterator.

```cpp title
--8<-- "{sourcedir}/01-hello-world/hello-world-05.cpp"
```

Using `std::copy` has allowed us to simplify the solution. We no longer need to hold the result of `end()` in a variable and there is no need to manually operate on the iterators. The algorithm does all the `*` and `++` heavy lifting for us. This may seem like an overkill for this case, but the goal is to make use of the existing facilities. Reinventing the wheel can be a viable solution if an extreme optimization is needed. In any case, the people writing the algorithms for the *STL*, including several specializations, can write much better code than we do.

Unfortunately, the trailing comma is still there.

```
Hello, World!,
```

Expecting anything else would have been naive because `std::copy` is using our instance of `std::ostream_iterator`, as we did before.

## A Custom Output Iterator

This is where extra thinking and development are needed to square the circle: a custom output iterator! Although one has to be humble and admit it will only be a wrapper around the `std::ostream_iterator` functionality. But what a mighty wrapper because it will take over the management of the delimiter.

```cpp title
--8<-- "{sourcedir}/01-hello-world/hello-world-06.cpp:6:35"
```

In order to be useful, the wrapper has to be template based to mimic the original. It can then provide the operations that one will manually use, as we saw with `*out++ = val;`, and account for the possibility of other operations being used by the algorithm inside the *STL* operation. That is why both `++` increment operators, prefix, and postfix, are available. Those and the `*` dereference operator are simple NOP commands.

The real work is done with the `=` assignment operator, when the iterator is given the value that has to be output. That is where the decision to output the delimiter is done. And it is done just before outputting the value, for all assignments except for the first, which is controlled with a boolean, `m_dodelim`. The *STL* outputs the delimiter after the fact, we do it before, changing the logic.

Anyone who also knows Python will notice that we are replicating the `'delimiter'.join(iterable of str)` functionality, that creates a string by joining the iterable with the delimiter in between elements, except after the last element. Even better, we do not need our range to be made out of strings, because of the generic functionality templates offer us. Granted, in Python the iterable of strings can be quickly achieved with `(str(x) for x in iterable)`.

Defining the `iterator_category` in our custom iterator is compulsory and the alias we set it to, is the obvious choice: `std::output_iterator_tag`, because that is what our custom iterator and the wrapped one are doing: output.

One final note about this custom iterator. The usual implementation of the `++` postfix operator looks like this.

```cpp
    auto operator ++(int) { auto tmp = *this; ++(*this); return tmp; }
```

But because our `operator *()` and `operator ++()` are *NOP* implementations returning a reference to itself, creating a `tmp` copy is not needed and we can implement `++(int)` also as an *NOP*.

The new main function that goes with it looks like this.

```cpp title
--8<-- "{sourcedir}/01-hello-world/hello-world-06.cpp:37"
```

This delivers the expected solution output, with no trailing `, ` delimiter at the end.

```
Hello, World!
```

But more importantly, our solution is just one step away from one of the declared goals: be *STL*-like. The general form we will be looking for is the one taking an input range and accepting also the beginning of a destination (or output) range.

## An STL-like Solution

It may seem like something redundant to go any for it, because `std::copy` is already part of the *STL*, but let us go for our own *STL*-like solution.

```cpp title
--8<-- "{sourcedir}/01-hello-world/hello-world-07.cpp:37"
```

Our newly defined `hello_world` has the final form we will be trying to use for each and every solution. We have obviously used `I` for *"Input"* and `O` for *"Output"* to make things as clear as possible.

## Summary

A simple 1-statement solution has been turned into a basic model for future solutions. By converting the non-input (constant value) into an iterable input range, wrapping the output iterator into our own custom iterator, we are able to control the presentation and model the final solution function after the *STL* algorithms.

Mission accomplished! Well, one of many missions to come.
