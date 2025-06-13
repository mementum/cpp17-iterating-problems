#include <array> // std::array
#include <iostream> // std::cout
#include <iterator> // std::ostream_iterator

int
main(int, char *[]) {
    auto hello_world = std::array{"Hello", "World!"}; // input range
    auto out = std::ostream_iterator<std::string>{std::cout, ", "}; // with delim
    for(const auto &hw: hello_world) // range based for loop
        *out++ = hw; // val to output and move output
    return 0;
}
