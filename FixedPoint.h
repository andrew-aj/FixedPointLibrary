#ifndef FIXEDPOINTMATH_FIXEDPOINT_H
#define FIXEDPOINTMATH_FIXEDPOINT_H

#include <bitset>
#include <cmath>
#include <utility>
#include <iostream>
#include <concepts>

template<int T>
concept correctSize = requires(){ T <= 18; T > 0; };

template<typename T>
concept is_pair = requires(T t) {
    typename T::first_type;
    typename T::second_type;
    t.first;
    t.second;
    requires std::same_as<decltype(t.first), typename T::first_type>;
    requires std::same_as<decltype(t.second), typename T::second_type>;
};

template<typename T>
concept integer_pair = is_pair<T> && std::integral<typename T::first_type> &&
                       std::unsigned_integral<typename T::second_type>;

template<int decimalPoints> requires correctSize<decimalPoints>
class DecimalFixedPoint {
public:
    //Base case for 10^0 when calculating the power at compile time.
    template<std::size_t decimal = decimalPoints>
    static constexpr typename std::enable_if<decimal == 0, int64_t>::type power() {
        return 1;
    }

    //Compile time calculation of 10^decimalPoints using enable_if to recursively calculate.
    template<std::size_t decimal = decimalPoints>
    static constexpr typename std::enable_if<decimal != 0, int64_t>::type power() {
        return 10 * power<decimal - 1>();
    }

    uint64_t ipower(int exponent) {
        return (exponent == 0) ? 1 : 10 * ipower(exponent - 1);
    }

    //Returns the amount of precision for this class.
    constexpr std::size_t getPrecision() {
        return decimalPoints;
    }

    constexpr int64_t getStorage() {
        return storage;
    }

    const std::string to_string() {
        auto mod = storage % power();
        return std::to_string(storage / power()) + '.' + std::to_string(mod > 0 ? mod : mod * -1);
    }

    inline int digits(std::integral auto num) {
        return log10(static_cast<double>(num)) + 1;
    }

    DecimalFixedPoint() {
        storage = 0;
    };

    DecimalFixedPoint(std::integral auto input) {
        storage = static_cast<int64_t>(input) * power();
    }

    DecimalFixedPoint(std::floating_point auto input) {
        int64_t cast = static_cast<int64_t>(input) * power();
        storage = cast;
        if (input > 0) {
            storage += static_cast<int64_t>((input - (cast / power())) * power());
        } else {
            storage += -1 * static_cast<int64_t>((input + (cast / power())) * power());
        }
    }

//    interesting algorithm for calculating digits of an integer
//    unsigned int baseTwoDigits(unsigned int x) {
//        return x ? 32 - __builtin_clz(x) : 0;
//    }
//
//    static unsigned int baseTenDigits(unsigned int x) {
//        static const unsigned char guess[33] = {
//                0, 0, 0, 0, 1, 1, 1, 2, 2, 2,
//                3, 3, 3, 3, 4, 4, 4, 5, 5, 5,
//                6, 6, 6, 6, 7, 7, 7, 8, 8, 8,
//                9, 9, 9
//        };
//        static const unsigned int tenToThe[] = {
//                1, 10, 100, 1000, 10000, 100000,
//                1000000, 10000000, 100000000, 1000000000,
//        };
//        unsigned int digits = guess[baseTwoDigits(x)];
//        return digits + (x >= tenToThe[digits]);
//    }

    DecimalFixedPoint(integer_pair auto input) {
        storage = static_cast<int64_t>(input.first) * power();
        auto mod = static_cast<int64_t>(input.second) % power();
        if (input.second < power()) {
            if (input.first > 0) {
                storage += mod;
            } else {
                storage += -1 * mod;
            }
        } else {
            int dig = digits(input.second) - decimalPoints;
            if (input.first > 0) {
                storage += static_cast<int64_t>(input.second) / ipower(dig);
            } else {
                storage -= static_cast<int64_t>(input.second) / ipower(dig);
            }
        }
    }

    DecimalFixedPoint(DecimalFixedPoint<decimalPoints> const &input) {
        storage = input.storage;
    }

//    DecimalFixedPoint operator*(DecimalFixedPoint const &input) {
//        int scale = ipower(decimalPoints);
//        DecimalFixedPoint<decimalPoints> temp;
//
//        //Here we only divide by the scale factor of the other number
//        temp.storage = (storage * input.storage) / scale;
//        return temp;
//    }

    template<int decimal>
    DecimalFixedPoint operator*(DecimalFixedPoint<decimal> const& input){
        int scale = ipower(decimal);
        DecimalFixedPoint<decimalPoints> temp;

        //Here we only divide by the scale factor of the other number
        temp.storage = (storage * input.storage) / scale;
        return temp;
    }

private:
    int64_t storage;

    template<int decimal> requires correctSize<decimal>
    friend class DecimalFixedPoint;
};


#endif //FIXEDPOINTMATH_FIXEDPOINT_H
