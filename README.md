# C++17 - Iterating Problems

This repository contains C++ code (up to C++17) to solve basic problems creatively and the content associated to it to produce a PDF book or a mkdocs-material site. See here, where you can read the content and download the PDFs.

  - https://mementum.github.io/c++17-iterating-problems

## Using the Code

You need `make` (GNU Make) and `gcc`, version 8 or greater (the project has used GCC 11.4.0). The *Makefile** restrict the standard to be C++17. If you are using something else you will need to modify the building and execution system

The code can be found under the `code` directory. There you can do:

  - `make all` and all challenges will be built
  - `make ab` and all the solutions for the challenge with the numeration `ab` will be be built.
  
Challenges are named using this convention: `ab-fancy-name`.

Each challenge has several solutions named like: `fancy-name-xy.cpp`. Inside the folder of each challenge you can run this:

  - `make all` to compile, run and verify each solution
  - `make xy` to compile, run and verify solution `xy`.
  
Both at the `code`root level and inside each challenge you can simply run `make` (or `make help`) to see the options and supported by the `Makefile` (like for example. `show`, `showoutput`, `showdiff`) and what they do. Most of the solutions are meant to succeed. A couple are meant to show a concept but fail.

## Generating the Content

There is also a `Makefile` at the root of the project to generate the PDFs and the `mkdocs-material` site. Running `make` (or `make help`) will show all options and a help text for each of it. The most used commands in this project (used by the author of course) are

  - `make pdf-prepress` - Generates a PDF ready for the press, including front and back covers. *Amazon KDP* takes that same content without the covers (`make pdf-kdp`) plus a separate file for the covers to create a printed book.

  - `make mkdocs` - Prepares the content for the `mkdocs` site

  - `make mkdocs-serve` - Runs the local `mkdocs` HTTP server in the background, to see the changes every time `make mkdocs` is run.
  
The `mkdocs` functionality is managed in a virtual environment with `pdm`, but that is something the `Makefile` checks. If you have no `pdm` but `mkdocs-material` is installed in your system, things will work.

## Some Explaining

As a hobby programmer I always try to keep tabs with the latest C++ developments, having learnt the language before C++98 was a standard. But reading is not the same as practicing by writing code and embedding knowledge by overcoming challenges, being finally able to assimilate the concepts.

Back in 2016 I already brought myself I bit up to date, up to C++14, by solving problems on the now defunct *CodeEval*. However, it was more of a personal competition by solving the same problems in Python and C++. The C++ solutions could have been plain-C solutions in most cases, except for the usage of `std::cout` and `std::cin`.

Looking forward to using C++20, and even C++23, I looked for new platform and *HackerRank* seemed well suited until two things stopped me.

  - *HackerRank* looks like abandoned and the C++20 support is non-existent, even if listed in the drop-down menus.
  
    On the positive side that means the code editor is not locked to just editing a plain function.
    
  - Solving the C++ problems felt initially again like writing C++ code and the goal was not to solve the problems, but to learn C++ concepts.
  
Furthermore, by only writing code I was just spitting out code and not really burning knowledge and concepts in the *EPROM*. Yes in the year 2025 it is clear *AI* is going to replace all of us but as human being I still feel the need to challenge myself, learn and achieve my goals.

And my own version of [Literate Programming](https://en.wikipedia.org/wiki/Literate_programming) was conceived with these objectives

  - Remain on C++17 as supported by *HackerRank*. There will be time for updates to C++20, C++23 and C++26 when it is out.

  - Re-think how simple problems like *"Hello, World!"* can be reworked and evolved to something challenging to be able to apply C++ concepts.
  
  - Given that the problems have *"input"* and *"output"*, focus (as much as possible) on *Iterators* to solve the challenge and the evolutions I create. (Hence the name *"C++17 - Iterating Problems"*)
  
  - Write about it, presenting to my line of thought to any potential reader and how I came to the solution and the subsequent evolutions of it.
  
I had already created a framework to write a book using `markdown`, with the help of the [asciidoctor](https://asciidoctor.org/) to generate fantastic PDF versions and adding some little tinkering with a custom python pre/post-processing engine. It was a matter of making some adaptations to be able to also produce a nice website with [Material for MkDocs](https://squidfunk.github.io/mkdocs-material/).

Feel to point out any mistake in my C++ code, blatant errors or anything you think is worth telling. But be lenient and remember I am not a professional programmer, just a poor soul trying to improve.

Have fun!
