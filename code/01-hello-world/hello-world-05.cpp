#include <algorithm> // std::copy
#include <array> // std::array
#include <iostream> // std::cout
#include <iterator> // std::ostream_iterator

int
main(int, char *[]) {
    auto hello_world = std::array{"Hello", "World!"}; // input range
    auto out = std::ostream_iterator<std::string>{std::cout, ", "}; // with delim
    std::copy(hello_world.begin(), hello_world.end(), out); // copy range => out
    return 0;
}
