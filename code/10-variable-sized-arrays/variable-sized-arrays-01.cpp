#include <iostream> // std::cin, std::cout
#include <vector> // std::vector

int
main(int, char *[]) {
    // problem parameters (const and from input)
    int n, q; // number of arrays, number of queries
    std::cin >> n >> q;
    // define matrix, create and gather array elements
    auto vmatrix = std::vector<std::vector<int>>{};
    for(int k; n--;) {
        std::cin >> k;
        auto &vec = *vmatrix.emplace(vmatrix.end());
        for(int ki; k--; vec.push_back(ki))
            std::cin >> ki;
    }
    // run the queries
    for(int i, j; q--;std::cout << vmatrix[i][j] << std::endl)
        std::cin >> i >> j;
    return 0;
}
