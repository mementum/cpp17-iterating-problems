#include <iostream> // std::cin, std::cout

int
main(int, char *[]) {
    // problem parameters (const and from input)
    constexpr int mod = 1 << 31;
    int N, S, P, Q;  // variables for input
    std::cin >> N >> S >> P >> Q;
    // lambda to calculate next position
    auto fmove = [P, Q](const auto &x) { return (x * P + Q) % mod; };
    auto tort = S % mod, hare = tort; // initial positions
    // solve problem
    auto n = 0;
    while(++n < N and ((tort = fmove(tort)) != (hare = fmove(fmove(hare)))));
    std::cout << n; // output result and go
    return 0;
}
