#include <iostream>
#include <chrono>
#include "FixedPoint.h"

int main() {
    FixedPoint<8> temp = 1000;
    FixedPoint<8> t2 = 8;
    (temp / t2).print();

    return 0;
}
