# For Loop

**Title**: For Loop
**Link**: https://www.hackerrank.com/challenges/c-tutorial-for-loop/problem

Let us go for the next "inconsequential" challenge that we want to transform in the best next iterating challenge.

This one is a `for` loop in between the integers `a` and `b`, where those integers are read from `std::cin`. The output yields the equivalent integer as a word for anything `< 10` or else the parity of the given integer. An obvious approach to the problem would be as follows.

```cpp title
--8<-- "{sourcedir}/07-for-loop/for-loop-01.cpp"
```

To be really smart we use the left and right sides of the `const char *numbers[]` array, to have our `even` and `odd` results all stored within a single data structure.

## Going Python

Using some `std::istream_iterator<int>` and `std::ostream_iterator<std::string>` is not what we are looking for. Our goal has to be an iteration that removes the `for` loop and let us iterate from `a` to `b`, i.e.: we need something that generates all the integers.

We obviously do not want to create an `std::vector` that holds all the integers even if in the test case that goes with the code we have `8` and `11`. That will, for sure, not mean any kind of stress, but we could be considering millions of integers and the question is why should be storing them. If we consider Python, the builtin `range` can be used. For example (simplified example)

```python
    `\n'.join(numbers[i if i < 10 else 10 * (i % 2)] for i in range(a, b + 1))
```

Ranges in Python are half-open, hence the `b + 1` and the integer are not generated, then stored and finally delivered one by one. The function generates and yields an integer, delivering the next when requested. We know (living in the future) that C++20 has implemented something like this with: `std::views::iota(1, 10)` but recall we are still on C++17.

Let us therefore address the problem of "replicating" (what we need) the `range` function in C++. Because we want to pass the range to a function that transform the integers to a string, we need a range, i.e.: `first` and `last` iterators, that we will get from `begin` and `end` functions. What we are basically describing is a container, and in our case we are describing a *"virtual"* container because we are not going to contain anything, except the description of the range: `start`, `stop`. Although we could hardcode the step-by-step increment between those limits, let us be flexible by defining also a `step`, that will default to a value of `1`.

This is how the beginning of our container looks like.

```cpp title
--8<-- "{sourcedir}/07-for-loop/for-loop-02.cpp:7:13"
```

Notice how, right from the start, our container is a template defaulting to `int`. The idea is to allow using different types of integral types. To disallow using something like a `float`, we will later consider *SFINAE*. To make the range as *unlimited* as possible, the default value for the `stop` part is taken from the standard library with `std::numeric_limits<T>::may()`. Much better than writing fixed values.

The constructors are the next best addition to the sauce of this container.

  1. We have a `struct StartStopStep` defined, so let us use it as a direct input for a constructor. That would be akin to using a `slice` in *Python*.
  
  2. Given that we have already referred to *Python* a couple of times, nothing like defining constructors, that mimic the way the builtin `range` works.
  
    - Number of parameters: `1`. The value is used for `stop`. Our other parameters take default values, i.e.: `start = 0` and `step = 1`
    
    - Number of parameters: `2`. These are then, in order, `start` and `stop`, with `step` again defaulting to be `1`
    
    - Number of parameters: `3`. These are then, in order, `start`, `stop` and `step`be `1`
    
  Although we have three options for constructor family taking integers, we only need to define two of them. Let us see it.
  
```cpp title
--8<-- "{sourcedir}/07-for-loop/for-loop-02.cpp:15:19"
```

The last constructor covers the `2-parameters` and `3-parameters`, assigning a default value to `step`. This last constructor shows us something very interesting: a pseudo-parameter-by-name calling convention.

```cpp title
--8<-- "{sourcedir}/07-for-loop/for-loop-02.cpp:17:19"
```

Our internal member `m_sss`, an instance of `StartStopStep` is being initialized by referring to the members defined in it, just by prefixing the name with a `.` (a dot). It feels almost like *Python* but it is not. The magic is limited, but it is a nice example of how *"named parameters"* can make things very clear. Consider that *Python* ended up adding more types of parameters, like positional-only parameters, named-only parameters to give users a finer grained control when calling a function.

It is now time to see how this container defines the iterator that will go over the `StartStopRange`.

```cpp title
--8<-- "{sourcedir}/07-for-loop/for-loop-02.cpp:21:49"
```

The iterator needs to know how the range is defined and it will therefore hold a copy of it. We could have achieved the same effect by keeping a pointer to the container that instantiated the iterator. For practical purposes it is the same. But notice that if we do not hold that pointer, the container could be deleted and the iterator can still be valid.

Something that seems contradictory because if there is no container, after the deletion, one could ask what is what the iterator is iterating on. In our case, the answer is easy: on the virtual range `StartStopStep`, that is **not contained** anywhere, but rather **defined** in the container. If the iterator makes a copy of the range definition, it can directly use that definition to iterate.

The rest should be straightforward. There is an `m_pos` member that will control where the iterator is, used at the same time to directly hold the final position when instantiating the end of range iterator through the method `end`. The iterator will increase `m_pos`, if still possible, on each iteration, until the value defined by `stop` is reached.

It is time to show the entire solution. But before that, let us explain the other things we did. We have also:

  - Gathered input with an `std::istream_iterator<int>(std::cin)`, creating `auto` variables for `a` and `b`.
  
  - Used our container `Range` to define the iteration range `[a, b]`, which is a closed range including both `a` and `b`.
  
    Recall that we are emulating the *Python* version and our definition is half-open, `[a, b)`. Hence the need to instantiate the container with `range = Range(a, b + 1)`.
  
  - Put our solution in a separate method that takes an input range with `first` and `last` iterators and a destination `out` iterator.
  
  - Invoked our solution with `range.begin()` and `range.end()` and our already classic output iterator, i.e.: `std::ostream_iterator<std::string>(std::cout, "\n")`.
  
In our solution method we have done two things that differ from the previous simplistic approach. Because our objective is to reuse what people have developed over the years for the *STL*, we:

  - Changed the storage for the words to be an `std::array`. Recall that with C++17 we can abuse the compiler and avoid telling how many elements there are and what the type of the elements is.

  - Put the logic to decide the output word in a small *lambda expression*.
  
  - We need it to be able to use `std::transform(first, last, destination, unary_predicate)`. I.e.: our range will be iterated and each value will be passed as the single argument (*"unary"*) to our *lambda*/function (*"predicate"*). The result of the evaluation will be sent to the destination.
  
Our case is simple and things go to the standard output but we could be sending the result to an `std::vector` or the iterator could point to a network socket or send an *HTTP* request to a well-known *API*.
    
Here is the code.

```cpp title
--8<-- "{sourcedir}/07-for-loop/for-loop-02.cpp"
```

## Refining And SFINAE

Let us start with our `Range<T = int>`. Ideally we would accept anything that works like an integral type. For obvious reasons a `float` (or a `double`) may lead to failures due to the non-exact nature of floating point arithmetic. With that in mind, adding *SFINAE* for `Range<T = int>` looks like this.

```cpp title
--8<-- "{sourcedir}/07-for-loop/for-loop-03.cpp:8:12"
```

Note that we have change how we integrate our custom `enable_if`. Instead of delivering a type that will have a default value, `enable_if<x, y> = nullptr`, we use the return type as the input to an anonymous template parameter, i.e.:

```cpp title
template <typename T = int, typename = enable_if<x, y>>
```

Just a different way to use our *SFINAE* machinery.

There is something that we have to take into account because `std::is_integral<T>` will recognize integral types. However, user defined classes that work like integral types will not pass the test.

We would need this other check.

```cpp
template <typename T>
using enable_if_integral = std::enable_if_t<std::numeric_limits<T>::is_integer;

template <typename T = int, typename = enable_if_integral<T>>
class Range {
```

With a specialization added to recognize our user defined type. `std::is_integral` has an *undefined behavior* if specializations are added.

Furthermore, we also have this at the beginning of `Range`.

```cpp title
--8<-- "{sourcedir}/07-for-loop/for-loop-03.cpp:13:17"
```

I.e.: we source the maximum value of our integral type `std::numeric_limits<T>::max()` and we would need a similar construct (or a specialization) for our user defined `T`. Too much for the scope of our work.

With regards to the iterators, we could simply add the previous *SFINAE* machinery, fire and forget. However, we have something we can improve: our solution function could take iterators and a custom function to solve the problem. And that means we also have to check the signature of that function: parameters and return type. This is how we do it.

```cpp title
--8<-- "{sourcedir}/07-for-loop/for-loop-03.cpp:74:87"
```

We first fetch the type of the input iterator with `std::iterator_traits<T>::value_type` as an alternative to our previous formulation with `decltype(*std::declval<T>())`. This is what goes as the parameter to the function provided for the solution.

To get the return type we meet a new helping hand: `std::invoke_result<F, Args ...>` a new entry in C++17. Given a callable `F` and a set of types that will be provided as arguments for the call, we find out the return type at compile time.

With those two things in hand and having also an output iterator `O`, we use the `constexpr bool` templated setup to find out if the output iterator can take what the function returns after taking the type. For that we use again `std::void_t<type, ...>`. Recall that it will turn valid types into `void` and fail otherwise. Our type will be valid if our `std::declval` formulation inside `decltype` is valid: `std::declval<O>() = std::declval<i2f_type<I, F>>()`. I.e.: if the `operator =` of our output iterator can take the return type of the solution function, given the proper input type.

We put it all together with the standard checks to see if our template parameters are iterators (or at least look like them) and coupled with `std::enable_if`, we have a good barrier for wrong inputs.

```cpp title
--8<-- "{sourcedir}/07-for-loop/for-loop-03.cpp:89:96"
```

The full code contains four alternative formulations of functions for the solutions. Only one solves the challenge but that is not he point. The point being: two of them meet the *SFINAE* requirements and two of them fail them (wrong number of parameters and wrong return type) Feel free to play with them. The default is the function that solves the problem.

Here is the full code.

```cpp title
--8<-- "{sourcedir}/07-for-loop/for-loop-03.cpp"
```
