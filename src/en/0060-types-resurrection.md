# Types - Resurrected

**Title**: Basic Data Types\
**Link**: <https://www.hackerrank.com/challenges/c-tutorial-basic-data-types>

## Iterating Iterators

That oxymoron is simply there to remark that for the *"Basic Data Types"* challenge we actually iterated over the types and we did not see a single iterator along the way. In spite of all the wonders we applied, `if constexpr`, `sizeof...`, *fold expressions*, and `std::variant`, we were working in compile-time mode. There was no room for iterators.

But there is always room for improvement, and we are going to find room for the iterators.

## Templated Iterators

Obviously, if we have been working with templates to address the problem of moving around a list of types, using templated iterators seems like the way to go.

We are going to use two new *C++* elements, and as it has been the case, we are going to couple another *C++17* present with a couple of *C++* veterans. The modern element is `std::any`, a class capable of holding any value, that has to be retrieved with the proper type hint, using `std::any_cast<T>(any_instance)`. Our veteran will be `std::vector`, to store factories of iterators for different types. Those factories will be hosted by another *C++11* family member veteran, namely `std::function`.

Recall that we previously had different ways of iterating over the list of types. To build something more complex we still need a first iteration over them. We will be building upon the latest solution, using `std::variant` and looping with a `size_t i` template parameter.

Let us see how the factories and the storage for them are modeled.

```cpp title
--8<--
{sourcedir}/06-basic-data-types-resurrections/basic-data-types-resurrections-01.cpp:12:12

{sourcedir}/06-basic-data-types-resurrections/basic-data-types-resurrections-01.cpp:78:79
--8<--
```

For the input, there will be a vector holding functions that return `std::any` and take an `std::istream &` as the only parameter. The `std::vector` for the output will hold functions that take an `std::ostream &`, the value in the form of `std::any`, and finally two `size_t` parameters. We need those two to pass the values controlling the decimal places for `float` and `double`.

Let us see how we will use that for the input iterator, which we name `vistream_iterator` (where `v` stands for `variant`).

```cpp title
--8<--
{sourcedir}/06-basic-data-types-resurrections/basic-data-types-resurrections-01.cpp:14:26


{sourcedir}/06-basic-data-types-resurrections/basic-data-types-resurrections-01.cpp:43:47
--8<--
```

Just as we did previously, we recurse over the types stored in the `std::variant` we receive as a template parameter. For each type a *lambda* is created and stored in a vector. That *lambda* will later take over and create an `std::istream_iterator` of the corresponding type, returning it embedded in an `std::any` instance.

The key to using those *lambda* expressions is in the `*` and `++` operators.

```cpp title
--8<-- "{sourcedir}/06-basic-data-types-resurrections/basic-data-types-resurrections-01.cpp:53:61"
```

When someone invokes the `*` operator, the **next** *lambda** is retrieved. Literally, `std::next` is always called with an offset to the beginning of the container. That offset is incremented during `++` operations, checking if the end of the container has been reached.

There is of course a corresponding recursion procedure for the output.

```cpp title
--8<-- "{sourcedir}/06-basic-data-types-resurrections/basic-data-types-resurrections-01.cpp:81:103"
```

The code in the *lambda* does the same as we did previously. The only novelty, as is the case with the input, is the pre-creation of the *lambdas* that go into the `std::vector`. The `vostream_iterator` in which we have our storage, will later retrieve the corresponding *lambda*.

```cpp title
--8<-- "{sourcedir}/06-basic-data-types-resurrections/basic-data-types-resurrections-01.cpp:142:147"
```

As was the case with our previous custom output iterator the real work takes place in the `=` operator. Here also, `std::next` is used to fetch the *lambda* that knows what to do with the `std::any`, given also an output stream and the values for the decimal places for `float` and `double`.

It is now possible to create an *STL*-like solution. Here is the code for the entire solution.

```cpp title
--8<-- "{sourcedir}/06-basic-data-types-resurrections/basic-data-types-resurrections-01.cpp"
```

## Criticism

Even if we have made our code dynamic and *STL*-like, we have probably made things overly complex. Especially because there is an interdependency between the input and the output as they have to remain synchronized to ensure the output iterator retrieves the proper *lambda* to match the type stored in the `std::any` instance.

There must be a way to remove that link between the input and output iterators.

## Dynamic Type Output

Tackling the nature of the input iterator is not going to be easy because it has to respect the ordering of the target types. But for starters, we can do sensible things such as taking the generation of the input functions out of the constructor.

In the case of the output iterator we can go several steps beyond also doing that. `std::any` has a very interesting member function named `type`. As the name indicates it lets us know things about the type being held because it *"returns the typeid of the contained value"*. That means we get the following done for us: `typeid(type_in_std::any)`, returning the type information in an instance of `std::type_info`.

*C++11* brought us `std::type_index`, a wrapper around the latter to make it possible to use the type information as the key in mapping containers. And that is what is going to help us with a simplification of the output iterator. We first put the output functions in an `std::map` using an `std::type_index` as the key. When the time comes to do actual output, `std::any`, via `type()`, gives us a `type_info`, that we can wrap inside an `std::type_index` to look for the proper function in the `std::map`.

```cpp title
--8<-- "{sourcedir}/06-basic-data-types-resurrections/basic-data-types-resurrections-02.cpp:14:42"
```

We have gone the `using` way to have a clear view of the containers holding our input/output functions, the ones working directly with `std::any`. Iterating over the types of the `std::variant` used to hold the types, both the `std::vector` and the `std::map` are generated.

The input iterator does not change much because it still needs to know the number of types it has to fetch and it still keeps an index with `m_pos`.

On the other hand `vostream_iterator` is no longer a template based class.

```cpp title
--8<-- "{sourcedir}/06-basic-data-types-resurrections/basic-data-types-resurrections-02.cpp:87:89"
```

That also means there is no index to track in parallel to the index of the input iterator. We can now have a generic approach to finding the right function by retrieving it from the `std::map` with the information from the `std::any` instance.

```cpp title
--8<-- "{sourcedir}/06-basic-data-types-resurrections/basic-data-types-resurrections-02.cpp:114:130"
```

For bonus points, we also use `typeid(type)` to find out if the fixed point precision has to be set for `float` and `double`. We were previously doing that directly in the output procedures with `if constexpr`, but can do it dynamically here now thanks to the `type()` member function of `std::any`.

To make our life easier we use another of the goodies of *C++17*, i.e.: *Structured Bindings*. It feels almost like unpacking an iterable in Python.

```cpp title
--8<-- "{sourcedir}/06-basic-data-types-resurrections/basic-data-types-resurrections-02.cpp:123:125"
```

It also feels like magic, because we can unpack the `std::pair` held by the map directly into variables, that can be declared as `auto`. Just like in Python we use `_` as a placeholder for the variable we want to ignore. This will apparently be official in *C++26*, when one will be able to use officially use the placeholder and do so as many times as needed.

Let us see the complete code of this second solution.

```cpp title
--8<-- "{sourcedir}/06-basic-data-types-resurrections/basic-data-types-resurrections-02.cpp"
```

## Going The Iterating Mile

We started this new chapter because iterating over the list of types was not the goal. That is why we now have solutions based on iterators. Unfortunately, these solutions rely on knowledge of the `std::variant` and the containers with helpers to fetch types from `std::cin` and put them to `std::cout`.

A real success would be to remove those two dependencies and make the code even more generic. And yes, we can!

Let us start by backpedaling a bit. We decided to go for `std::any` as the vehicle to pass instances of the types back and forth, because it can hold anything. However, that flexibility comes with a price: getting the value out of an `std::any` instance is hard, hence the need to have so many *lambda expressions*, why we synchronized the iterators first and had to work with `std::type_info` later.

Let us look at three ideas to streamline our solution

  1. Given that we are using `std::variant` to convey the types and it can also hold instances of the types, it may be a lot easier to work with it.

  2. Let our iterator, literally iterate over the container, instead of doing something with the container. I.e.: our custom iterator will work with iterators to traverse the container of *lambda expressions*.

  3. If getting the value out of an `std::variant`, with the right type, is easier than with `std::any`, we may remove the *lambda expressions* for the output and make the output iterator really generic.

The code generating the *lambdas* for fetching the values from `std::cin` does not change. It is still an `std::vector`, holding *lambdas*, but this time the return value is an `std::variant`. Let us show it.

```cpp title
--8<-- "{sourcedir}/06-basic-data-types-resurrections/basic-data-types-resurrections-03.cpp:12:24"
```

No *lambdas* are generated for the output.

The input iterator works with a `typename I`, that must be an iterator.

```cpp title
--8<-- "{sourcedir}/06-basic-data-types-resurrections/basic-data-types-resurrections-03.cpp:31:62"
```

It is used during construction to get the beginning, with `first`, and the end, with `last`, of the range that holds procedures to fetch the types. Our input iterator no longer needs to know things about the container. Because the range is not directly controlled by our iterator, the second constructor that defines the end of iteration also needs to know where the end of the real range is. And that it is why it takes also a `last` parameter.

Granted, with some *SFINAE* we could restrict and check that the iterator `I` is of type *InputIterator* and delivers the expected result when dereferenced.

----

Things are even better for the output iterator. We had already decoupled it from the `std::variant` but it still had to rely on a set of external functions (*lambdas*) to output each type, as the type had to be forcefully extracted from `std::any`. With the use of `std::variant` things are a lot cleaner.

```cpp title
--8<-- "{sourcedir}/06-basic-data-types-resurrections/basic-data-types-resurrections-03.cpp:100:120"
```

Everything happens inside the `operator =` method and the reason is that extracting the type from an `std::variant` is not that difficult, because we have the help of new and known friends. First

  - `std::visit` that will put into a function of our choosing the argument inside the `std::variant` (or a collection of those)

If we use a generic *lambda*, i.e.: with an `auto` parameter, the type will be worked out for us. But we still need the type and:

  - `decltype` comes to the rescue to get the type of `auto &arg`

  - Coupled with a new friend: `std::decay_t`. This fine piece of machinery removes the *reference* and gives us the real type behind `auto &arg`

With the type in the hand, we can again resort to using our compile-time friend and wonder, `if constexpr`, to apply specific code for `float` and `double` as we did in the past.

There is something worth mentioning, the `template <typename V>` technique. Because that `V` is the parameter to the method. Recall that the input methods will return an `std::variant` with the value embedded. Our `operator =` does not need to know the types in the template instantiation; this will be worked out by the compiler.

What we could do is add a bit of *SFINAE* to make sure that `V` is an `std::variant`, using `std::enable_if_t` and a check for the type.

Let us see the complete code of this final solution.

```cpp title
--8<-- "{sourcedir}/06-basic-data-types-resurrections/basic-data-types-resurrections-03.cpp"
```

Done and dust. We have really generic code for both `vistream_iterator` and `vostream_iterator`, both freed from having to have knowledge of the `std::variant`.

## Summary

The goal in this chapter was to solve the conundrum of using iterators to iterate over types, rather than fully iterate over the types at compile time. And our solutions have also been subject to iteration until we reached that goal, as much as possible.

## Bonus Points

OK, if you have made it that far, you have seen the cliffhangers about adding *SFINAE* for the new things. We could not really leave the chapter without adding it.

First, we need to check if the code generating the *lambda* for type conversion from an `std::istream` is really getting an `std::variant`. This is the trickiest because one cannot really check for `std::variant` because it will always be bound to a template pack.

The trick is therefore to specialize a `std::false_type` / `std::true_type` check, where the true part is a specialization containing an `std::variant<Args...>`.

```cpp title
--8<-- "{sourcedir}/06-basic-data-types-resurrections/basic-data-types-resurrections-04.cpp:18:34"
```

`vistream_iterator` is the next one in need of *SFINAE* to see if the template parameter `I` is really an *InputIterator* and even more: if it delivers a function returning an `std::variant` after being dereferenced and called with an `std::istream`. Because that is the expectation when `vistream_iterator` is dereferenced itself.

```cpp title
--8<-- "{sourcedir}/06-basic-data-types-resurrections/basic-data-types-resurrections-04.cpp:45:64"
```

Recall that lately we had made a new friend, `std::decay_t<T>`, and it comes to the rescue again. But not alone, because it needs a push from the left, i.e.: being prefixed by `typename` to be able to finally get down to `::result_type`.

This is because `::result_type` is a dependent name itself, only available after the previous conundrum has been solved. `std::function` stores there the return type of the stored function, and that is what we are looking for.

After that comes our `template <V> vostream::operator =(const V &v) const`. That `V` needs to be a variant. We therefore reuse what we already developed above and apply `enable_if_variant<V>`.

```cpp title
--8<-- "{sourcedir}/06-basic-data-types-resurrections/basic-data-types-resurrections-04.cpp:132:133"
```

For the general case, we have reapplied the rest of the standard *SFINAE* machinery, to check for input/output iterators that also have compatible types and was being used in the first solution.

It is now that we have a really complete solution. Game over and here is the code.

```cpp title
--8<-- "{sourcedir}/06-basic-data-types-resurrections/basic-data-types-resurrections-04.cpp"
```
