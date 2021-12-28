#include <iostream>
#include <chrono>
#include "FixedPoint.h"

int main() {
    int first, second;
    std::cin >> first;
    std::cin >> second;
    auto t1 = std::chrono::system_clock::now();
    FixedPoint<5> a(first);
    FixedPoint<5> b(second);
    a = a * second;
    auto t2 = std::chrono::system_clock::now();
    a.print();
    auto dur = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1);
    std::cout << dur.count() << std::endl;
    t1 = std::chrono::system_clock::now();
    float out = (float)first * second;
    t2 = std::chrono::system_clock::now();
    std::cout << out << std::endl;
    dur = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1);
    std::cout << dur.count() << std::endl;
    return 0;
}
