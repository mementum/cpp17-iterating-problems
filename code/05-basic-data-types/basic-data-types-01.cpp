#include <iomanip> // std::setprecision
#include <ios> // std::fixed
#include <iostream> // std::cin/cout

int
main(int, char *[]) {
    int i;
    long l;
    char c;
    float f;
    double d;

    std::cin >> i >> l >> c >> f >> d;

    std::cout << i << std::endl;
    std::cout << l << std::endl;
    std::cout << c << std::endl;
    std::cout << std::fixed << std::setprecision(3) << f << std::endl;
    std::cout << std::fixed << std::setprecision(9) << d << std::endl;

    return 0;
}
