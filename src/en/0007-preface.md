# Introduction {epub:type=preface}

In our modern times and with the advent of *Artificial Intelligence*, there are many prophets that have loudly announced the end of programmers as a species. And they are probably right. The exact moment in time still remains a mystery.

Until then, a hobby programmer, like your humble author, can still decide to tackle some programming challenges using *C++*. This is something I have done in the past to keep myself updated with the incredible changes that have introduced into the language since the *C++11* standard was brought to life.

Considering that the last serious period of time was back in 2016, it was already time to bring myself up to date. My first intention was to use *CodeEval*, the platform I used in the past, but the platform is gone for good. The second obvious choice was *HackerRank*, simply because I tested it a couple of times and my account was still there.

Seeing *C++20* as one of the choices, skipping the *C++17* wave seemed like a good choice. Unfortunately, the *C++20* selection was simply a label from a `clang` compiler version with a couple of features from the end of 2017, and for sure not the next best standard sanctioned in 2020.

On the bright side, *HackerRank* does not lock the editor for this *C++20* label. It does in some, not all, instances of problems for previous versions. This is something I could use to my advantage to provide complete solutions, rather than solving the problem by filling out just one method.

This gave me an idea, that later lead to this book/blog/static site, call-it-x if you wish. I could rethink the problems and address them as something much bigger, forcing myself along the way to work with as many features of *C++17* as possible. And as it usually happens in life, one idea led to the next: solve the challenges using iterators as much as possible.

The *STL* (Standard Template Library) was already a thing when I had my first contact with *C++*, even if *C++98* was yet to be conceived. Back then, I hit a wall when I first tried to love iterators, until one day something made *"click"* and the idea seemed simply but incredibly brilliant. Hence the thought of solving everything now with iterators, even a simple *"Hello, World!"* challenge.

## The Chapters and Their Content

Each chapter presents several solutions to a given challenge. Even more, a challenge can be revisited in a second chapter to go above and beyond. Usually, the simplistic, obvious and boring solution is presented first as the starting point. After that, several solutions are presented, showing different approaches and/or the evolution to a generic iterator based solution. 

Not all the solutions are meant to succeed in solving the problem. Some of them are meant to show where a human being, the author, fails at quickly identifying what one must do.

If a solution introduces new concepts, be it from *C++17* or previous standards, it is specifically mentioned. That is the whole point of the exercise, apply a variety of techniques and the tools of the *C++* arsenal, embedding that knowledge deep in one's brain.

I have obviously read blogs and quickly scanned books and seen online *C++* online reference guides over the years. And I have used the reference guides during the writing of this text. Because I doubted how to do a correct usage of something, to see alternatives and to explore the member functions.

But I want to say something: if I have not understood a concept or technique, I have not used it. Copying code is something even AI can do, but that would not bring me personally any further. It is not about solving the problem with someone else's code and idea, it is about doing it myself, because I understand how to do things.

Savvy *C++* programmers will obviously identify several areas of improvements, obvious error and even things that work due to sheer luck. They should not hesitate to provide feedback on the repository as that will bring one step closer to better understand and use *C++*. As a hobby programmer of course, but a programmer nonetheless.

/// note
In other cases I simply left things a bit untidy, because my other life (the real one) has a higher priority and an incredible gravitational pull. Please, be lenient with this humble coder.
///

Let me finish by saying that writing this in the form of a book has really helped me. Telling a story and wanting to improve the solutions to be generic and iterator based, has improved my focus and made me a better *C++* hobby programmer++. Or so do I believe, because as a kid I thought I was so good I could play professional soccer and that never ever crystallized. 

## Navigating The Code

The code for the chapters follow the chapter numbering and has this form: `xy-descriptive-text`, where `xy` is a series of two digits that matches the chapter in which the solutions in that folder are developed and shown.

Going inside the folder each solution has also this form: `descriptive-text-ab.cpp`. Once again the `ab` placeholder stands for two digits, following the order in which the solutions are presented in the chapter.

One can test each solution inside the folder with: `make ab`. This will compile `descriptive-text-ab.cpp` and execute the binary `descriptive-text-ab` (with `.exe` under *Windows*), feeding it a test case and evaluating the output against the well-known result.

Beware: the `Makefile` uses *GNU Make* extensions, to look for files, parse names, generate the tare

A failed test case will show the expected output, the actual output and the difference of the two. This can be forced for positive test cases with `make show ab`. If in doubt, just run `make` or `make help` to see the available options (like an obvious `make all` to compile and test all solutions)

That was for each individual challenge. Executing `make xy` from the top directory, will enter the `xy-descriptive-text` directory and compile and test all solutions `make all`. At this level a `make all` is also possible to compile and test all the solutions for all the challenges.

If you add a new solution following the aforementioned conventions, the `Makefile` will automatically be able to compile and test it. That is: `make ij` will work, where `ij` are the digits you will have assigned to your solution with this convention: `xy-descriptive-test/descriptive-test-ij.cpp`.

## Final Words

Hopefully someone finds something interesting here and not only the author enjoys the content and the process of having crafted it.
