#include <iostream> // std::cout

int
main(int, char *[]) {
    const char *numbers[] = { // we do not need the zero index, a >= 1
        "even", "one", "two", "three", "four",
        "five", "six", "seven", "eight", "nine", "odd"
    };

    int a, b;
    std::cin >> a >> b;
    for(int i = a; i <= b; i++)
        std::cout << numbers[i < 10 ? i : 10 * (i % 2)] << std::endl;

    return 0;
}
