# Types, Types, Types

**Title**: Basic Data Types\
**Link**: <https://www.hackerrank.com/challenges/c-tutorial-basic-data-types/problem>

This title probably sounds so bad that even Ballmer's "Developers, Developers, Developers ..." gig would have sold many more vinyl records.

The next challenge gives us a list of basic types. We are expected to read values from the standard input and output the values back to the standard output, with some formatting in a couple of cases. Let us go for it.

## The Obvious Approach

```cpp title
--8<-- "{sourcedir}/05-basic-data-types/basic-data-types-01.cpp"
```

We simply create some variables of the following types: `int`, `long`, `char`, `float` and `double`, read from `std::cin` and output to `std::cout`. `float` and `double` have some manipulation to with a fixed number of decimals each, namely 3 and 9.

A simple replacement of this approach with five `std::istream_iterator<T>` and another set of five `std::ostream_iterator<type, "\n">` is for sure not the kind of challenge we are looking for, even if one of the goals is to use iterators here, there and everywhere. Let us therefore skip putting that in written form.

## The Classic Recursive Template

If this were Python, we could iterate over the type names, because those type names are also the classes that we can instantiate to obtain values. But this is not Python and we cannot iterate over the types.

Unless we can. If we consider that one can pass an unlimited number of types to a template and we can then pass those types to another template function, it would seem as if we could iterate. Let us craft a `for_each` looping function. And let us do in the most classic way.

```cpp title 
--8<-- "{sourcedir}/05-basic-data-types/basic-data-types-02.cpp:41:46"
```

The tricks here should be easy to see, `for_each` takes any number of types by using a template parameter pack, yes, the thing with the `...`. At the same time, and to avoid ambiguity, two of the parameters are specified before the pack. We actually need two, because the pack could be empty and there would be no way to differentiate between these two cases.

```cpp
// 1 template parameter ... the pack is empty
template<typename BasicType, typename ...BasicTypes>

// 1 template parameter
template<typename BasicType> //
```

If the pack is empty, the compiler resolves both cases to be simply a template with one parameter and cannot decide which one will be the match. But if we have two, we are making sure that if the pack is empty, the cases are different.

```cpp
// empty pack, 2 template parameters
template<typename BasicType, typename BasicType2, typename ...BasicTypes>

// 1 template parameter
template<typename BasicType> //
```

That is how the two recursive calls to `for_each` can succeed. The rest will be easy, given that we only need the following.

  - A generic `for_each` that instantiates an `std::istream_iterator<BasicType>` and an `std::ostream_iterator<BasicType, "\n">` and copies one element from the former to the latter.
  
  - Because `float` and `double` need some special handling, specializations for those types are needed.
  
  - A call in `main` to start the iteration.

All those ideas expressed in code are shown here.

```cpp title 
--8<-- "{sourcedir}/05-basic-data-types/basic-data-types-02.cpp"
```

Even if it solves the problem, there are some ugly things. We need to carry around the number of decimal places, `3` and `9`, for `float` and `double`, and to avoid warnings we need to suppress the parameter name. It is not nice.

```cpp title 
--8<-- "{sourcedir}/05-basic-data-types/basic-data-types-02.cpp:9:9,17:17,30:30"
```

A lot of code is also repeated in the management of the precision for `float` and `double` and in the instantiation of the iterators. Let us address those shortcomings.

## The Classic Template Reworked

```cpp title 
--8<-- "{sourcedir}/05-basic-data-types/basic-data-types-03.cpp"
```

This is our best try. A function `in_out` serves as a central factory for the `std::istream_iterator` / `std::ostream_iterator` pair we need. And the management of the `float` / `double` precision differences is also outsourced to a `for_each_floating` function.

But that is all. The recursive template nature and the specializations are still there. There has to be something more modern than this.

## The "if constexpr" Express

*C++17* introduces a real beast: `if constexpr`, i.e.: we can logically reason during compilation time. It is not that we were not already doing it. The template solutions we presented before were choosing what function to call in the background and with which types, so the logic was there.

But not being able to implement the logic directly with `if ...` was taking the power out of our hands, and *C++17* changes the landscape. Here is the solution after our new white elephant comes into the picture.

```cpp title 
--8<-- "{sourcedir}/05-basic-data-types/basic-data-types-04.cpp"
```

It really feels like magic. Adding two lines of logic to handle `float` and `double` does remove the need for separate generic and specialized template solutions. And with it the need to duplicate code or to have additional factories and helper functions.

Our, already old and well-known, friend `std::is_same_v` enables the `if constexpr` check. Remember that we discarded `std::is_same` for our iterator checks, because several types (subclasses) of iterators could fit the profile. In this case we want to be specific.

The world is already a better place, but rest assured it can be evaluated from many different perspectives.

## The "if constexpr" Single Method Solution

The introduction of `if constexpr` has allowed a quick removal of specializations and all the bloat (or optimizations) we added. But we still have two template functions. One is doing the work, and the other is simply managing the recursion.

Given the powers granted to, and by, `if constexpr`, it is easy to wonder if the recursion can also be managed by this wonder of wonders. Let us answer that question by directly looking at the following code.

```cpp title 
--8<-- "{sourcedir}/05-basic-data-types/basic-data-types-05.cpp"
```

Yes and **no** (a classic German "Jein", i.e.: "Ja" + "Nein"). `if constexpr` alone cannot enter godmode. It needs to work in tandem with a pre-existing member of the *C++* family: `sizeof...(pack)`. As the name implies, it allows us to know the number of elements in the pack. Now, we may even perform a logical `if` check on it to know if recursion is needed.

That is the key to remove the trick of having the *"2 parameters + 1 pack"* trick we analyzed before. `for_each` now features a standard *"1 parameter + 1 pack"*. And before recursing, it is checked if the pack has any remaining types. This is really clean, because the *"1 parameter"* has a clear name, `BasicType`, for its usage in the function and `...BasicTypes` contains the remaining types, if any, that would still need to be processed.

## The Fold Expression Miracle

Good things come in pairs, and *C++17* did not simply bring `if constexpr`, it also brought *Fold Expressions* to the table. A miracle that needs some consideration before one can really understand its depth.

Reading about it and looking at many examples and ideas about how *Fold Expression* work, one of the approaches seemed like magical. Actually, more magical than the others. It was the application of *Fold Expressions* with the comma operator. In a regular case, the `,` operator works by executing the statements that are separated by the comma and returns the value of the last one. Like in this `while` situation.

```cpp
    int a = 0, b = 100;
    while(a++, a < b)
        do_something();
```

After increasing `a` with `a++`, the boolean value of the comparison `a < b` is what will determine if the `while` loop continues or not. But with *Fold Expressions*, this is different:

```cpp
template<typename ...ParameterPack>
auto
my_function() {
  (do_something<ParameterPack>(), ...);
}
```

Our parameter pack is being folded over the `,` operator, i.e.: each of the members of the pack is being passed one by one to `do_something<ParameterPack>()`. We can for sure work with that. Let us see how.

```cpp title 
--8<-- "{sourcedir}/05-basic-data-types/basic-data-types-06.cpp"
```

Recursion is managed by *Fold Expressions* with the `,` operator. We have seen ourselves in the need to outsource the processing of the type again to the `in_out` function. And we are in the business of less is more.

## The Fold Expression Single Method

This is where another modern member of the *C++* family comes to the rescue, because we can make good use of a *lambda expression*, i.e.: an unnamed function declared there where you need it. We can move the content of `in_out` inside our `for_each` to have it working together with the *fold expression*.

But some reworking will be needed, because a *lambda expression* cannot use templates. OK, because we live in the future we know this is possible with *C++20*, but our compiler is a veteran still using *C++17*. Luckily, not all hope is lost, because we can invite a well-known friend to the party: `decltype`. This time it comes without the famous partner `std::declval<T>`, but this time extra help in deducing the type is not needed, because it is partnering with someone else: `auto`. In this case as a parameter type, for the first time ever ... in our *C++* ramblings, of course.

Because a template parameter is not possible for the *lambda*, we have crafted an automatic parameter.

```cpp title 
--8<-- "{sourcedir}/05-basic-data-types/basic-data-types-07.cpp:11:11"
```

The parameter will `auto`-magically adapt itself to what we pass. It is not that we have invented this, because it was invented with *C++14*, with the concept being named a *generic lambda*.

Notice how we also have `fprec` and `dprec` in the *lambda expression*. They are captures, hence the lack of a default value, because it is captured from the surroundings to be used inside the *lambda*.

To make our *Fold Expression* cooperate with the lambda, we need to make a small change. In the previous example, we were invoking `in_out<BasicTypes>()`, because we had a templated function. Not being able to use templated *lambda expressions*, the best we can do is to pass a value of the type, by instantiating the type.

```cpp title 
--8<-- "{sourcedir}/05-basic-data-types/basic-data-types-07.cpp:28:28"
```

A value that can be later turned back, for our purposes, into a type with `decltype`.

```cpp title 
--8<-- "{sourcedir}/05-basic-data-types/basic-data-types-07.cpp:15:15"
```

And here is the complete single method approach with a *Fold Expression*.

Here is the single method, thanks to the cooperation of all those heroes.

```cpp title 
--8<-- "{sourcedir}/05-basic-data-types/basic-data-types-07.cpp"
```

There is something worth mentioning before we carry on. Because the *Fold Expression* is managing the iteration of the parameter pack, this parameter pack is the only parameter to the template. No tricks with a first parameter or even two parameters to differentiate the candidates to be matched. A real improvement.

```cpp title 
--8<-- "{sourcedir}/05-basic-data-types/basic-data-types-07.cpp:8:8"
```

## The Last Crown Jewel - std::variant

We need nothing more until we realize we do, because *C++17* just keeps on giving. It introduced a type-safe union that can hold any value. One at a time, of course. One only needs to specify the types that may be inside `std::variant` as template parameters.

It would seem as if someone was thinking about our little problem here when they designed this functionality, because that someone also added an `std::variant_alternative<size_t, MyVariant>`. I.e.: we can retrieve the type specified at a given position for an `std::variant`.

Somehow we need to pass a `size_t` to our template function and check if we have come to the last type or else recurse. The pieces of the puzzle are all there:

  - templates do not only take types, they also take values.
  
  - `if constexpr` has already helped us with the logic to control the recursion.
  
With all the accumulated experience from the previous attempts, we can do this in one go.

```cpp title 
--8<-- "{sourcedir}/05-basic-data-types/basic-data-types-08.cpp"
```

Our looping `for_each` now takes only one template parameter for the types, the `std::variant` that carries those types. It takes a second parameter `size_t i = 0`, to let us know the current type to be retrieved. 

```cpp title 
--8<-- "{sourcedir}/05-basic-data-types/basic-data-types-08.cpp:15:15"
```

And retrieved it is. That is the key to having things working. Notice how we have to use a strange notation with `i`.

```cpp title 
--8<-- "{sourcedir}/05-basic-data-types/basic-data-types-08.cpp:28:28"
```

Being `size_t i` a template parameter, it is "read-only" for the function and we cannot simply do `i++`, because it is not an *lvalue*, hence the need to use `i + 1` for the recursive call.

## Summary

We have not iterated over types in a Python-like style. But we did it, not only with a classic recursive template approach. We have added as much as possible from the weaponry provided by *C++17* to let us have single method approaches that control the recursion internally and reduced the template parameters to manage all types with a single one.

It is time to bury this topic, although there may be more to come.
