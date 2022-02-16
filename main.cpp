#include <iostream>
#include <chrono>
#include "FixedPoint.h"

int main() {
    FixedPoint<3> sqrt = FixedPoint<3>::sqrt(100000);
    sqrt.print();

    return 0;
}
