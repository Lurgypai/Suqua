#include <iostream>
#include "Pool.h"

static void printPoolValues(Pool<int>& pool) {
    for(auto& v : pool) {
        std::cout << v << ' ';
    }
    std::cout << '\n';
}

int main(int argc, char** argv) {

    Pool<int> testPool;
    printPoolValues(testPool);
    std::cout << "Empty print\n";

    testPool.add(0, 0);
    printPoolValues(testPool);
    std::cout << "EXPECTING 0\n";

    testPool.add(9, 9);
    printPoolValues(testPool);
    std::cout << "EXPECTING 0 9\n";
    
    testPool.add(5, 5);
    printPoolValues(testPool);
    std::cout << "EXPECTING 0 5 9\n";

    testPool.add(10, 10);
    printPoolValues(testPool);
    std::cout << "EXPECTING 0 5 9 10, resize triggered\n";

    testPool.add(11, 11);
    printPoolValues(testPool);
    std::cout << "EXPECTING 0 5 9 10 11\n";

    testPool.add(21, 21);
    printPoolValues(testPool);
    std::cout << "EXPECTING 0 5 9 10 11 21 resize triggered\n";

    testPool.free(21);
    printPoolValues(testPool);
    std::cout << "EXPECTING 0 5 9 10 11 post free\n";

    Pool<int> testPool2;
    for(int i = 0; i != 10000000; ++i) {
        testPool2.add(i, i);
    }

    Pool<int> testPool3;
    testPool3.add(3617, 3617);
    printPoolValues(testPool3);
    std::cout << "Expecting 3617\n";

    testPool3.free(3617);
    printPoolValues(testPool3);
    std::cout << "Expecting \"\"\n";

    return 0;
}
