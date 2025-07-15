# Input And Output - Reloaded

**Title**: Input and Output\
**Link**: <https://www.hackerrank.com/challenges/cpp-input-and-output>

If the Matrix franchise could reload itself, so can we, improving what we have so far done.

## A Better Input And Output

We ended this challenge with two small problems: the `auto`-return type and the extra `init` parameter that we wanted to be generic, yet compatible with the types the iterators manage.

### Deducting Types

Let us first address the second one. Recall that `std::iterator_traits<T>` is the generic interface giving us access to the definitions iterators have to carry. And amongst those definitions we have `value_type`, i.e.: a type. It is the perfect mix: iterators and an API to fetch the type inside the iterator. `init` can now be a lot more generic.

```cpp title
--8<-- "{sourcedir}/03-input-and-output-reloaded/input-and-output-reloaded-01.cpp"
```

This is exactly one of those moments when Italians scream: "Mamma Mia!". That is really verbose but it works. If we pass something that is not directly convertible to `int`, in our example, there will already be complains in the IDE and compilation will of course fail.

But there is a better way by using a couple made for each other: `decltype` and `std::declval<T>`. Although they belong to different generations, *C++11* / *C++14*, they are both jewels. The former lets you deduct the type of the expression passed to it, so one can dynamically calculate, for example a return type or a parameter type. The latter can take an unknown (unevaluated) type `T` to use member functions of it during compilation: i.e.: not going through constructors.

We know that iterators have a `*` operator to retrieve the current value. We can therefore use `std::declval<T>` to access that value by pseudo-constructing `T` and applying the operator.


```cpp
    *std::declval<I>() // Pseudo-constructed T, use the* operator to get a value
```

Once we have a value `decltype` can give us the *declared type*, hence the name.

```cpp
    decltype(*std::declval<I>()) // Get declared type of the "value"
```

It the right moment to apply this to the code in the challenge.

```cpp title
--8<-- "{sourcedir}/03-input-and-output-reloaded/input-and-output-reloaded-02.cpp"
```

And things look a lot friendlier for the casual code reader and reviewer. Not only that, the errors reported by the IDE and the compilation process are shorter and more amicable. And although both solutions work, we will stick, wherever and whenever possible, with the `declxxx` dynamic duo, because it offers a general interface for deducting types, whereas `std::iterator_traits<T>` is limited to iterators.

### Regaining The "auto" Return Type

By using *SFINAE* with `std::enable_if_t` and an *enabler*, i.e.: return type from it, we lost the `auto` facility we were applying to our solution functions. We are forced to manually specify the right type in `std::enable_if_t`.

But see, there is a trick because `std::enable_if_t` returns a type and that means we could put that in the template declaration, where types are allowed. If it fails, the template will not be part of the candidate set. If it succeeds, the template will be a possible match for the template instantiation we look for. Here is how it looks (no changes to `main`)

```cpp title
--8<-- "{sourcedir}/03-input-and-output-reloaded/input-and-output-reloaded-03.cpp"
```

Recall that the default *enabler* type for `std::enable_if_t` is `void`. And that is what we therefore effectively are left with in the code if the checks are all `true`, is this:

```cpp
template <typename I, typename O, void* = nullptr>
auto
input_and_output(I first, I last, O out, decltype(*std::declval<I>()) init = {})
```

Our third template parameter is valid, of type `void *` and has a default value of `nullptr`. We do not really care about this third parameter because its only purpose is to succeed in being there or fail, in order to remove itself from the candidate set.

Notice that we are back in business because `auto` is back in business itself, as the return type. This saves us from the burdensome work of having to specify the right type there or as the second parameter to `std::enable_if_t<bool, typename = void>`.

The match candidate will finally look a bit more awkward once the types are in place. Similar to the code below, where we can easily recognize what goes where, how `int` was deduced for `init` and how our `nullptr` enabler is ignored.

```cpp
template<>
void
input_and_output<
    std::istream_iterator<int, ...>, // I
    std::ostream_iterator<int, ...>, // O
    nullptr> // std::enable_if result
(
    std::istream_iterator<int, ...> first,
    std::istream_iterator<int, ...> last,
    std::ostream_iterator<int, ...> out,
    const int &init
)
```

Both small problems are now solved.

### SFINAE On Steroids

Steroids is probably an exaggeration. Let us simply say that we can improve the look of the *SFINAE* checks and make things generic for its reusability later somewhere else.

We are indeed checking the nature of our iterators, but whether anyone is willing to type all that every time and actually read it later to understand what is going on, it is another story. Let us then present `std::void_t<class ...>` a *C++17* metafunction looking forward to helping us. It transforms valid types passed as templates arguments into `void`. If something is not valid, `std::void_t` will not transform it and this will be the key to remove that candidate.

To profit from `std::void_t` we will use the following technique:

  - Use a `struct` that can resolve to `true` if `std::void_t` succeeds in converting all types to `void`

  - Have the same `struct` as a fallback, with default template parameters, that will resolve to `false`. This will be a match if the checks above failed.

To have something that resolves to `true` and `false`, we may use `std::true_type` and `std::false_type`, the perfect *Yin* and *Yang* for the work. Both are specializations of `template<class T, T v> struct integral_constant`, where `T = bool` and each specialization receives the `v = true` and `v = false` values respectively.

For the sake of an example, let us imagine that we want to check if something is simply an iterator with `std::iterator_traits`, disregarding if it is an input, output, forward, or any other type of iterator. Using all the ingredients gathered above, this is how we would do it.


```cpp title
--8<-- "{sourcedir}/03-input-and-output-reloaded/input-and-output-reloaded-04.cpp:6:11"
```


This is probably easy to understand: the first template candidate is a subclass of `std::false_type` (its value will be `false` when instantiated), whereas the second, a subclass of `std::true_type` that will be `true` when instantiated. The second will be a valid candidate if the template parameter `T` has the iterator trait `iterator_category.` In that case `std::void_t` will convert that to `void`. If not it will fail and our `std::false_type` derived struct will be the matching candidate.

We could now apply this together with `std::enable_if`

```cpp title
--8<-- "{sourcedir}/03-input-and-output-reloaded/input-and-output-reloaded-04.cpp:6:18"
```

It seems like if we have gone from 100 to 0 in microseconds, removing most of the bloat. Notice that we use `{}` to instantiate `is_it<T>` and get the corresponding `true` or `false` value for the logical condition. This is equivalent to having used the `is_xxx_v` form above, a shorthand for `is_xxx::value`. Our `is_it` checker has actually a `::value` (declared as `constexpr static`), because it is a subclass of either `std::false_type` or `std::true_type`.

We can do even better and check multiple parameters in one go by using another *C++17* pearl, `std::conjunction<class ...>`. This one takes many types and performs a logical `and` test, returning the value in, you guessed, `::value`. As our types all have a boolean `::value`, the can be used for the logical conjunction.

```cpp title
--8<-- "{sourcedir}/03-input-and-output-reloaded/input-and-output-reloaded-05.cpp:13:14"
```

And this leads to an improvement/simplification of the previous code.

Our code can be further simplified.

```cpp title
--8<-- "{sourcedir}/03-input-and-output-reloaded/input-and-output-reloaded-05.cpp:16:20"
```

The goal, of course, is to check specifically if the iterators are subclasses of *InputIterator* and *OutputIterator*. Let us formulate these traits with `std::void_t` and then pack everything with `using` to make things even cleaner.

First the check for an *InputIterator*.

```cpp title
--8<-- "{sourcedir}/03-input-and-output-reloaded/input-and-output-reloaded-06.cpp:16:23"
```

Notice that we have used `std::is_base_of` and not the `_v` helper to get the value. `std::void_t` is expecting types and not boolean values.

We can do the same for the output.

```cpp title
--8<-- "{sourcedir}/03-input-and-output-reloaded/input-and-output-reloaded-06.cpp:25:32"
```

And then check both as shown above with `std::conjunction`.

```cpp title
--8<-- "{sourcedir}/03-input-and-output-reloaded/input-and-output-reloaded-06.cpp:34:35"
```

For bonus points and in order to get the gold medal for packaging and reusability, we can even alias the usage of `std::enable_if_t`.

```cpp title
--8<-- "{sourcedir}/03-input-and-output-reloaded/input-and-output-reloaded-06.cpp:37:38"
```

This means that our definition of `input_output` looks like this now.

```cpp title
--8<-- "{sourcedir}/03-input-and-output-reloaded/input-and-output-reloaded-06.cpp:40:44"
```

Although this looks really nice, we can still make it nicer. Using `decltype(*std::declval<I>())` is for sure a niece piece of work but we could rethink how we use `std::enable_if_t` (or the alias we have created) as a template parameter.

For starters we could alias it, as we have done with the other checks above.

```cpp title
--8<-- "{sourcedir}/03-input-and-output-reloaded/input-and-output-reloaded-07.cpp:40:47"
```

Even nicer. But we have not reworked `std::enable_if_t`. And something nicer would still check the traits but making something useful out of the third template parameter. Now that we can so nicely deduce the type inside iterator with `decltype` / `std::declval` the spark is there. Let us use that type as the return type in `std::enable_if_t` and then use that as a useful third template parameter instead of having just a dummy `nullptr` placeholder.

```cpp title
--8<-- "{sourcedir}/03-input-and-output-reloaded/input-and-output-reloaded-08.cpp:37:50"
```

We even added a bonus to replicate what we see in the standard our own `_v` helper with `io_iterators_v`. Instead of being a `struct` or a `using` alias, this one is a `constexpr bool`, i.e.: a bool calculated during compile time. That means that we do not have to initialize `io_iterators` or access the `static constexpr value` in it with `::value`, because our helper is doing it for us.

### Leaner and Meaner

Everything works, but not really fully. The typename `I_Type` has a default value in the template declaration but we are not specifying the template parameters: the compiler is doing that for us. That means that the real type passed as `I_Type` for the `init` parameter can be something else.

This will be caught by the compiler if the type is not convertible to the type inside the *InputIterator*, but the goal was to invalidate the candidate if the type is wrong.

We can additionally remove some of the bloat we have generated by using `constexpr bool` as we are wrapping `_v` values in `structs` that are subclasses of `std::true_type` plus a fallback subclass of `std::false_type`. If the standard `_v` helpers already give us `true` and `false`, let us then use them directly.

```cpp title
--8<-- "{sourcedir}/03-input-and-output-reloaded/input-and-output-reloaded-09.cpp:1:23"
```

There it is. Although `std::void_t`, `std::conjunction` and using the duo `std::false_type` / `std::true_type` looked good and it actually worked, we now need a lot less and remember the famous axiom: *"less is more"*.

Good old `bool` values that are calculated during compilation, hence `constexpr`, is everything we need. Notice that the type deduction for `init` with `it_type` remains unchanged and we have changed the name of `enable_if_io` to the typename we gave, and will still give, to the parameter `init`, i.e.: we have named the enabler as `I_Type`. See how this is applied in our new definition of `input_and_output`.

```cpp title
--8<-- "{sourcedir}/03-input-and-output-reloaded/input-and-output-reloaded-09.cpp:25"
```

It would seem as if we were playing a game of *"Where is the ball?"*. We first had our enabler `std::enable_if_t` in between the template declaration and the function definition. Later we moved it to be the default value of a template parameter (wrapping it in our own `enable_if_io`) And with the name `I_type` it is now the type for the `init` parameter in the function definition.

The `std::enable_if_t` check behind the curtains remains the same and the candidate will be eliminated from the set if either `I` and/or `O` fail the check to see the nature as an *InputIterator* and an *OutputIterator* respectively. What we are doing here is taking advantage of the fact that `std::enable_if_t` returns a type and that type is the one we want to have for `init`.

Recall that type was a hindrance at the beginning, because it would prevent us from using `auto` as the return type. It is now the perfect enforcer for the parameter type. Any wrong type passed to `init` will be caught early, in the definition of our function. In the previous version, any wrong parameter was flagged later, at the `std::accumulate` level.


## Summary

We started with our first ugly and bloated version of *SFINAE* and although we used nice constructs and incredible facilities from the language and the library, this only led to replacing bloat with bloat. We then progressed to make a leaner and meaner version that shall serve us as a model for the future.
