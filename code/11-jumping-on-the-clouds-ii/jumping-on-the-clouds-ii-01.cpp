#include <algorithm> // std::copy_n
#include <iostream> // std::cout/cin
#include <iterator> // std::istream/ostream_iterator
#include <vector> // std::vector

template <typename I, typename O>
auto
final_energy(I first, O out, int n, int k, int e) {
    auto cloud = first;
    do {
        auto i = std::distance(first, cloud); // >= 0
        cloud = std::next(first, (i + k) % n); // next is at (i + k) % n
        e -= 1 + (*cloud * 2); // jump - 1, on non-zero cloud == 1 .. +2
    } while(cloud != first);
    *out++ = e;
}

///////////////////////////////////////////////////////////////////////////////
// Main
///////////////////////////////////////////////////////////////////////////////
int
main(int, char *[]) {
    constexpr auto e = 100;  // starting energy level
    auto in = std::istream_iterator<int>{std::cin}; // input iterator
    auto in_last = std::istream_iterator<int>{}; // input iterator end
    auto out = std::ostream_iterator<int>{std::cout, "\n"}; // output iterator
    for(; in != in_last; in++) { // resync "in" after copy_n operation
        auto n = *in++, k = *in++; // input parameters
        auto c = std::vector<bool>{}; // storage
        std::copy_n(in, n, std::back_inserter(c)); // copy input
        final_energy(c.begin(), out, n, k, e);
    }
}
