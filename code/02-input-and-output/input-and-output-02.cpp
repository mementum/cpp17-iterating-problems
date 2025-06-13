#include <iostream> // std:cin/cout
#include <iterator> // std::istream_iterator/ostream_iterator

int
main(int, char *[]) {
    auto in = std::istream_iterator<int>{std::cin};
    auto out = std::ostream_iterator<int>{std::cout};
    *out++ = *in++ + *in++ + *in++;
    return 0;
}
