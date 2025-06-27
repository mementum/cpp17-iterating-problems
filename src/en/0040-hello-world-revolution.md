# Hello World - Revolutions

**Title**: Hello, World!\
**Link**: <https://www.hackerrank.com/challenges/cpp-hello-world> 

After the Matrix was reloaded a revolution followed. Let us try to revolutionize the *"Hello, World!"* challenge.

## SFINAE for the custom iterator

When we modeled our *"Hello, World!"* solution with iterators, including a custom output iterator, we had not yet introduced *SFINAE*. Let us see if our custom iterator can pass the test using the tools we have developed, recovering the `enable_if_io` naming, as we are not using the helper for a type declaration.

```cpp title
--8<-- "{sourcedir}/04-hello-world-revolution/hello-world-revolution-01.cpp:38:52"
```

Using that arsenal we rework the `hello_world` definition to use `enable_if_io`.

```cpp title
--8<-- "{sourcedir}/04-hello-world-revolution/hello-world-revolution-01.cpp:54:58"
```

And here comes the surprise, because after crafting that nice, even if verbose, piece of code, the custom output iterator that we crafted before **does not pass** our own check, and we for sure used the right tag. And we are checking with `std::iterator_traits` as we did in the previous chapter to verify the tag that went into the iterator.

```cpp title
--8<-- "{sourcedir}/04-hello-world-revolution/hello-world-revolution-01.cpp:15:15"
```

There will be complaints about `std::basic_string` (i.e.: `std::string`) not having an `iterator_category` defined. And that simply means that `std::iterator_traits` is trying to tell us that we were lazy.

One has to wonder if we really implemented an output iterator. Before *C++17* one could subclass from `std::iterator` and avoid being lazy, because all needed definitions had to be provided. But this was deprecated and a custom iterator needs to have some extra definitions in place to make `std::iterator_traits` happy.

Even more. Our laziness meant we were sloppy and defined the delimiter as `T m_delim`. This does not match what an `std::ostream_iterator` does, because it takes a `const char *`. Let us therefore fix those two things in the custom iterator, presenting the changes.

```cpp title
--8<-- "{sourcedir}/04-hello-world-revolution/hello-world-revolution-02.cpp:7:23"
```

The delimiter type issue requires some extra machinery, with a second `std::ostream_iterator` in place. We store the delimiter as an `std::string` and even if in our challenge, the values fed into our custom iterator are also of type `std::string`, they could be anything. And getting our delimiter into that other iterator would fail spectacularly.

To work cleanly we create an alias for each of the `std::ostream_iterator` instances we are hosting, namely: `ositer` for the main type to output and `osdelim` for the delimiter outputting the iterator.

Notice that we copy as many of possible of the iterator definition values from the iterator we are wrapping, namely: `iterator_category`, `value_type` and `difference_type`. But for `pointer` and `reference` the actual values returned by the corresponding methods are returned, i.e.: this iterator.

After this rework, the custom iterator passes all checks and the code compiles. Having been lazy had one advantage: now we know that the *SFINAE* machinery we developed previously works in all cases. We found out as soon as we applied it to the first version of the custom iterator.

## Checking Type Compatibility

In our previous case we had an `init` parameter for which we wanted to set the allowed type in its declaration as a parameter. We achieved this by returning the type gotten via `decltype` / `std::declval` with `std::enable_if_t`.

When considering the checks for this case, testing if the types of the input iterator can be converted to something accepted by the output iterator seems the logical way to go. That would have been the major price: packing that check into the final alias.

After all, we wrote this previously when we were solving the *"Input and Output"* challenge.

```cpp
    *out++ = *in++ + *in++ + *in++;
```

It would therefore seem logical that the type of `I` is convertible to something accepted by `O`. Once again, `decltype` and `std::declval<T>` come to the rescue, as we can pseudo-construct objects and use their functionalities.

Let us write a check for that.

```cpp title
--8<-- "{sourcedir}/04-hello-world-revolution/hello-world-revolution-03.cpp:64:75"
```

We first look for the type of the output, but simulating the input to output operation. In plain English: we are taking the type after the dereferenced declared value of `O` can take the dereferenced value of `I`. Shorthand: `*out = *in` but written as `*std::declval<O>() = *std::declval<I>())` If this works, `decltype` will deliver a type.

To process the type we recover `std::void_t`. Recall that we were previously able to work only with `constexpr bool` as we had values, but we are now deducting a type and have to resort to the `std::false_type` / `std::true_type` combo of structs we name `io_i2o`, i.e.: "input-output, input to output".

*"Go with the flow"*, *"Follow the Path of Least Resistance"*, *"Be Water My Friend"* or any other similar saying is telling us to go with the trend. And the trend favors the use of `_v` helpers. That is why we "degrade" the struct combo down to a `constexpr bool`, as we did previously to have our `io_i2o_v` helper.

The helper is added as a check to `std::enable_if_t` as the second term of a logical and, in which first we check the *input/output* nature of the iterators. If we changed the input `std::array` to hold integers instead of strings, there would be no candidate matching our `hello_world` function request, because the template parameter deduction would fail. One cannot directly put `int` into a `std::string`.

But wait: we are giving the output iterator a manipulated value and the outcome of the manipulation could be anything, even the instance of a custom class, after we have read some integers. Perfectly valid use cases that we would break by using our `io_i2o_v` as a check. The fact that we get have `const char *` values as the input and output a `std::string` is just a coincidence, it is a specific trait of this problem and not a general check.

We could therefore use it here for the sake of it, but not as a general rule, in we probably want to check if the types of the iterators match our expected definition.

The final code in its entirety is presented now.

```cpp title
--8<-- "{sourcedir}/04-hello-world-revolution/hello-world-revolution-03.cpp"
```

## Summary

We had a working solution, but all the work done with out *SFINAE* machinery has paid off, letting us improve our solution to be correct and not only a working solution. *C++17* has finally a proper *"Hello, World!"* salutation in place.
