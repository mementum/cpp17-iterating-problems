#include <iostream> // std::cout
#include <iterator> // std::ostream_iterator

int
main(int, char *[]) {
    auto out = std::ostream_iterator<std::string>{std::cout};
    *out++ = "Hello, World!";
    return 0;
}
