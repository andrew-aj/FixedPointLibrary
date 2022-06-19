#include <iostream>
#include <chrono>
#include "FixedPoint.h"

int main() {
    DecimalFixedPoint<6> a(std::pair<int,unsigned int>{-12345, 12345678});
    std::cout << a.to_string() << std::endl;
    return 0;
}
