#include <iostream> // std::cin, std::cout

int
main(int, char *[]) {
    int N; // gather array size and prepare it
    std::cin >> N;
    int arr[N]; // NON-STANDARD C++ - Variable Length Array Extension

    for (int i = 0; i < N;)  // input loop
        std::cin >> arr[i++];

    while(N) // reverse output loop
        std::cout << arr[--N] << " ";

    return 0;
}
