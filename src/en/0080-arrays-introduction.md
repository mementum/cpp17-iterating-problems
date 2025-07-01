# Arrays Introduction

**Title**: Arrays Introduction\
**Link**: <https://www.hackerrank.com/challenges/arrays-introduction/>

## Variable Length Arrays

Read some integers, store them in an array, and output them in reverse order. It does not sound difficult. Let us see what we can do.

Remember we said that one of the reasons to use the problem sets available at *HackerRank* was the fact that it seemed partially abandoned. The editor was therefore not locked, and we could redo the entire solution, including the input and output—parts that sometimes seem written by very motivated but inexperienced coders.

In this case, the coder tells us the following:

> Unlike C, C++ allows dynamic allocation of arrays at runtime without special calls like malloc(). If `n = 10`, `int arr[n]` will create an array with space for `10` integers.

Your humble author may not be the best coder in the world, but he surely knows something: *Variable Length Arrays* (aka *VLA*) are not part of the *C++* standard.

Furthermore, and unlike stated, the *VLA* functionality is part of the *C* standard. It has been part of it for many years. The question is whether the statement (at least for *C++*) can be proven to be true. Let us give it a try.

```cpp title
--8<-- "{sourcedir}/08-arrays-intro/arrays-intro-01.cpp"
```

Blistering Barnacles! It does work. Notice how we have marked the *VLA* line with the following comment: *"NON-STANDARD C++ - Variable Length Array Extension"*. Because it is a compiler extension. Originally, only *GCC* supported it, but it is also supported by *clang* today. It is now clear how the author came to the conclusion that *C++* supports *Variable Length Arrays*: he/she gave it a try and it simply worked. This may (and likely will) break with other compilers, of course.

/// note
This solution `01` can be compiled as `make 01 pedantic`. The `pedantic` target deactivates compiler extensions and that means that the build process will fail.
///

## Real Standard Variable Length Arrays

Let us see how we do the same but using proper *C++*.

```cpp title
--8<-- "{sourcedir}/08-arrays-intro/arrays-intro-02.cpp"
```

Rather than trusting that our compiler will understand, support, and properly manage *VLAs*, we resort to using the `new` and `delete` operators. Actually, we use the array variants: `new[]` and `delete[]`. These are real veterans of the *History of C++* and were added to the language in 1985.

We are using `new`, `delete`, and `std::cin` and `std::cout`. Replace those with `malloc`, `free`, `scanf`, and `printf` and we would have *C* code. Not the solution we are really looking for, but it is a step in the right direction.

## Smart Pointers, Iterators and Algorithms

One of the pleasures of *Modern C++*, i.e., anything *C++11* and newer, is that language designers understood that new facilities were needed to ease the burden on coders. Someone asked the rhetorical question about manual allocation/deallocation of memory for pointers/arrays and why that could not be managed with intelligence. And `std::unique_ptr<T>` was born (there is a `Deleter` template parameter that I am omitting here). As it happened with the `_v` variants of compile-time checks that help people, someone thought that no matter how good our `std::unique_ptr` was, a helper would be even better. *C++14* brought `std::make_unique`.

Let us use unique pointers together with iterators and algorithms.

```cpp title
--8<-- "{sourcedir}/08-arrays-intro/arrays-intro-03.cpp"
```

A unique pointer manages the allocation and deallocation of memory for us. We request an array of `N` (input parameter) integers and we get it. But the *STL* algorithms do not know how to work with smart pointers. Although it "contains" a pointer, it does not have the classic `begin` and `end` methods to give us iterators. And for good reasons: it is holding a pointer to a memory location. The interpretation of that is really down to the programmer. Nothing prevents us from trying to access the `N + 1` memory location, and we will probably crash the program if we do so.

But as we show in that example, we can still use the `n`-limited version of the copying algorithm, `std::copy_n`, and use the actual pointer that we retrieve with `std::unique_ptr<T>::get()`.

Even better: we can use that pointer to get a *reversed* iterator with `std::make_reverse_iterator` to traverse the array backwards as requested by the problem description. When iterators launched, they were touted as glorified pointers—and seeing that construction, we probably want to agree.

Let us state what the problem is: `N` is present everywhere. And we want to have a solution as generic as possible.

## Generalizing Smart Pointer Usage

Although we still are going to use `N`, we are going to move to using `std::copy`. First, by reading integers until we hit the end of the input stream. Nothing fancy here.

For the output, we are going to use a couple of helpers to define the beginning and the end of the array, to be able to make iterators out of it with `std::make_reverse_iterator`. The abstraction will help us, although we will still need `N`.

```cpp title
--8<-- "{sourcedir}/08-arrays-intro/arrays-intro-04.cpp"
```

We are getting closer to a general solution. `abegin` (for *"array begin"*) gives us the proper abstraction to understand that we can get a position within the array with `std::next`—the last one, obviously, by moving the pseudo-begin by `N` positions. If you are wondering why `std::begin` is not being used here, the answer is simple: it does not work. The official reason is that the end of the array (pointer-based) cannot be known and therefore `std::end` cannot be calculated to have a `begin` to `end` traversal.

## Going Vector

The obvious approach to overcome all the aforementioned limitations is to go `std::vector`. This allows us to mark `N` with the attribute `[[maybe_unused]]`, to let the compiler know that even if we have to fetch a value from `std::cin` and store it in `N`, we will be ignoring that variable.

First because we are reading until the end of the input to store things in our container. And later because our solution uses the iterators provided by the container to traverse the array backwards and solve the problem. We go backwards by using reverse iterators, with `rbegin` and `rend`.

```cpp title
--8<-- "{sourcedir}/08-arrays-intro/arrays-intro-05.cpp"
```

Notice how we use a new friend, `std::back_inserter(v)`, to insert elements at the "back" (i.e., using `push_back`) of the `std::vector`. This is our next target: generalizing the use of containers for the solution.

Something important is that we use the `::value_type`, using our `it_type` helper, of our input iterator to instantiate the proper version of `std::vector` and make sure that it can hold the input values.

## Let the Containers Come To Us

But only if needed. In the latest solution we assume a couple of things:

  - The input iterator will only go forward. That means we have to store the incoming integers to be later able to output them in reverse order.
  
  - That the ideal container to hold the integers (or whatever we may be storing) is an `std::vector`.
  
  - We can control the type that will be held in the container.
  
The other assumptions are the ones we solved using *SFINAE* in the previous chapters, to check if the input (direct or transformed) can go to the output, if the template parameters are iterators and, and, and.

Let us remove all those restrictions. For that, we are going to need the following things:

  - Lots of *SFINAE* checks.
  
  - Compile-time branch selection by checking the properties of the iterators and choosing the appropriate way to work with a container to hold the input values, if needed.
  
Because we are going to model our solution to even be able to work without storing the values.

Let us skip the well-known iterator checks we have from previous chapters and focus on a check we will repeat several times: `has_method_name`. We are going to see what API a given container has to select how to work with it. There are several `method_names` to check: `begin`, `end`, `rbegin`, `rend`, `push_back`, `push_front`, and `insert`. We will do it like this:

```cpp title
--8<-- "{sourcedir}/08-arrays-intro/arrays-intro-06.cpp:38:44"
```

For methods that take parameters, we have to add some `std::declval` complexity. See the case for `insert`:

```cpp title
--8<-- "{sourcedir}/08-arrays-intro/arrays-intro-06.cpp:94:105"
```

For the sake of checking things under a different light, we will be checking for subclassing in the case of `std::stack<T>`. This container adaptor is actually the perfect fit for the problem description: we push things into it, and the retrieval always happens in `LIFO` order. Notice that it is a *container adaptor* and not a container. It relies on other containers to provide the functionality. It does feature a different interface, and we would have needed to check three method names: `push`, `top`, and `pop`. But this also works for us:

```cpp title
--8<-- "{sourcedir}/08-arrays-intro/arrays-intro-06.cpp:107:110"
```

The `bool` value is directly provided by an old friend from previous chapters: `std::is_base_of_v`.

With all this in mind, let us see what is the logic to select different code branches.

  - If the input iterators are *Bidirectional Iterators* (or the superior *Random Access Iterators*), nothing will be stored. 
  
    Our solution function is only concerned with iterators and not where these iterators come from and how they have come to point to the realm of a container holding the values. Only the properties of the iterators are important.
    
    If they are bidirectional, we can reverse them with our `std::make_reverse_iterator` friend and skip storing any values ourselves. Let us see how easy it seems.

```cpp title
--8<-- "{sourcedir}/08-arrays-intro/arrays-intro-06.cpp:146:150"
```

With standard *Input Iterators*, we will need a container to temporarily hold the input values. Hence the basic definition (still no *SFINAE*) of our solution function.

```cpp
template <typename I, typename O, typename C = std::stack<it_type<I>>>
auto
reverse_function(I first, I last, O out) {
```

The default container is the magic `std::stack<T>` that perfectly fits the problem. But other options are possible, like using `std::vector`, `std::list` and others. Let us see the logic.

```cpp title
--8<-- "{sourcedir}/08-arrays-intro/arrays-intro-06.cpp:153:178"
```

Notice how the *C++17* beast `if constexpr` lets us formulate in an imperative and very natural form how we perform the selection of the different scenarios.

If the container is not our *LIFO* container, we check what insertion method is supported. Some containers support all three: `push_back`, `push_front`, and `insert`, whereas others support only the latter. Choosing to check `push_front` first means that we can output in reverse order by traversing the container from the beginning to the end, without reversing iterators. The other options will force us to use `rbegin` and `rend`, because the elements will be pushed at the back of the container.

Notice that we have added some informational messages instantiating an output iterator using `std::cerr`. This is because we can choose how to build this solution by defining `CASE0` through `CASE3` or letting the defaults be built.

Doing `make 06 case2`, for example, will use the standard input iterators and an `std::list`. This will in turn select a `push_front` approach and that will be shown with the message being pushed over the standard error stream. Like this:

```
$ make 06 case2
g++ -std=c++17 -DCASE2 -o build/arrays-intro-06 arrays-intro-06.cpp
[+]: Non-Bidirectional Iterator for the Input
[+]: Using push_front from container
./build/arrays-intro-06 < arrays-intro.input > /tmp/tmp.RgprHlfogT.output
Solution 06: SUCCESS


make: 'case2' is up to date.
```

Choosing `case0` will first store the values in an `std::vector` so the *Bidirectional Iterators* case plays along. No container will be instantiated inside the solution function.

```
$ make 06 case0
Creating build dir
g++ -std=c++17 -DCASE0 -o build/arrays-intro-06 arrays-intro-06.cpp
[+]: Bidirectional Iterator for the Input
./build/arrays-intro-06 < arrays-intro.input > /tmp/tmp.bfpOg7T5BO.output
Solution 06: SUCCESS


make: 'case0' is up to date.
```

Have a look at the `main` function below to see how the `CASEX` definitions choose different scenarios.

To support those code paths, we have to make sure that the containers support those things, and this is where the `has_method_name_v` and `is_stack_v` are used. Together with an extra check for type compliance between the container and the output operator.

```cpp title
--8<-- "{sourcedir}/08-arrays-intro/arrays-intro-06.cpp:112:142"
```

All that, and other checks, applied to the solution function with `std::enable_if` are of course shown at the end of the above snippet.

There is one new check that has been put in action above but for which no implementation has been shown: `is_bidir_v<T>` to test if an iterator supports moving in both directions. It uses the same machinery as the checks that test iterators to see if they are input or output iterators. See it below in the complete listing of this final solution.

If you cannot see the `insert` path being chosen you are right. There is no use case, in the code, to use it. But the solution is generic enough, so that it can be used with an `std::map`. The input values could be `std::pair` instances. A map has neither `push_front` nor `push_back` but does support `insert`, `rbegin` and `rend`. 

## Summary

We have gone from non-standard compiler extensions to create arrays and be bound by the size of the input, `N`, to using smart pointers to manage pseudo-iterators. Finally, we have come to show a solution that can even reverse the input without using intermediate storage and that can work with many different temporary containers.

---

```cpp title
--8<-- "{sourcedir}/08-arrays-intro/arrays-intro-06.cpp"
```
