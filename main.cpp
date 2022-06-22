#include <iostream>
#include <chrono>
#include "FixedPoint.h"

int main() {
    DecimalFixedPoint<3> a(std::pair<int,unsigned int>{123, 123});
    DecimalFixedPoint<3> b(std::pair<int, unsigned int>{456, 789});
    std::cout << (a * b).to_string() << std::endl;
    return 0;
}
