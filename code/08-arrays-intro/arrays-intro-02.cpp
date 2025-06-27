#include <iostream> // std::cin, std::cout

int
main(int, char *[]) {
    // gather array size and prepare it
    int N;
    std::cin >> N;
    int *arr = new int[N]; // standard c++ - dynamic allocation

    for (auto i = 0; i < N;) // input loop
        std::cin >> arr[i++];

    while(N) // reverse output loop
        std::cout << arr[--N] << " ";

    delete [] arr; // undo the dynamic allocation
    return 0;
}
