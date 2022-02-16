#ifndef FIXEDPOINTMATH_FIXEDPOINT_H
#define FIXEDPOINTMATH_FIXEDPOINT_H

#include <bitset>
#include <cassert>
#include <cmath>
#include <iostream>

template<std::size_t decimalPoints>
class FixedPoint {
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

    //Returns the amount of precision for this class.
    constexpr std::size_t getPrecision() {
        return decimalPoints;
    }

    //Returns the raw storage.
    const int64_t getStorage() {
        return storage;
    }

    //Default constructor
    FixedPoint() {
        static_assert(decimalPoints <= 18);
        static_assert(decimalPoints != 0);
        storage = 0;
    };

    //Function that converts a floating point type t to a fixedPoint format
    template<typename t>
    int64_t floatingPoint(t start) {
        assert(start <= (std::numeric_limits<int64_t>::max() / power()) &&
               start >= (std::numeric_limits<int64_t>::min() / power()));

        int64_t integer = (int64_t) start;
        int64_t decimal = (start - integer) * power();
        integer *= power();
        int64_t m_storage = 0;
        m_storage += integer;
        m_storage += decimal;
        return m_storage;
    }

    //Compile time branching to determine type of variable being converted to this class.
    template<typename t>
    inline void handler(t start) {
        storage = 0;
        if constexpr(std::is_floating_point_v<t>) {
            storage = floatingPoint(start);
        } else if constexpr(std::is_same_v<FixedPoint, t>) {
            storage = start.storage;
            return;
        } else if constexpr(std::is_integral_v<t>) {
            assert(start <= (std::numeric_limits<int64_t>::max() / power()) &&
                   start >= (std::numeric_limits<int64_t>::min() / power()));

            storage = (int64_t) (std::pow(10, decimalPoints)) * start;
        } else if constexpr(std::is_member_function_pointer_v<decltype(&t::getPrecision)>) {
            storage = handleDiff(start);
        } else {
            []<bool flag = false>() { static_assert(flag, "Wrong data type"); }();
        }
    }

    //Handles conversion from a different fixedPoint object of different precision.
    template<typename t>
    inline int64_t handleDiff(t start) {
        constexpr int64_t varpower = start.power();
        constexpr int64_t thPower = power();
        if constexpr(varpower > thPower) {
            int64_t diff = varpower / thPower;
            int64_t decimal = (start.getStorage() % varpower) / diff;
            int64_t integer = (start.getStorage() / varpower);
            return integer * thPower + decimal;
        } else if constexpr(varpower < thPower) {
            int64_t diff = varpower / thPower;
            int64_t decimal = (start.getStorage() % varpower) * diff;
            int64_t integer = (start.getStorage() / varpower);
            return integer * thPower + decimal;
        } else {
            return start.getStorage();
        }
    }

    //Converts type t to a fixed point.
    template<typename t>
    FixedPoint(t start) {
        static_assert(decimalPoints <= 18);
        static_assert(decimalPoints != 0);
        handler(start);
    }

    //Prints the number in normal decimal format.
    void print() {
        std::cout << (storage / power()) << "." << (std::abs(storage) % power()) << std::endl;
    }

    //Equality operator overloading.
    bool operator==(const FixedPoint &rhs) const {
        return storage == rhs.storage;
    }

    //Opposite of equality overloading.
    bool operator!=(const FixedPoint &rhs) const {
        return !(rhs == *this);
    }

    //Assignment operator overloading.
    template<typename t>
    FixedPoint &operator=(const t &rhs) {
        handler(rhs);
        return *this;
    }

    //Assignment operator overloading for pair of {integer, decimal} numbers.
    FixedPoint &operator=(const std::pair<int64_t, uint64_t> majorMinor) {
        assert(majorMinor.first <= (std::numeric_limits<int64_t>::max() / power()) &&
               majorMinor.first >= (std::numeric_limits<int64_t>::min() / power()) && majorMinor.second < power());
        storage = majorMinor.first * power() + majorMinor.second;
        return *this;
    }

    //Addition operator overloading.
    template<typename T>
    FixedPoint operator+(const T &rhs) {
        FixedPoint returnVal;
        if constexpr(std::is_same_v<FixedPoint, T>) {
            returnVal.storage = (storage + rhs.storage);
            return returnVal;
        } else if constexpr(std::is_integral_v<T>) {
            returnVal.storage = (storage + rhs * power());
            return returnVal;
        } else if constexpr(std::is_floating_point_v<T>) {
            int64_t toFixed = floatingPoint(rhs);
            returnVal.storage = (storage + toFixed);
            return returnVal;
        } else if constexpr(std::is_member_function_pointer_v<decltype(&T::getPrecision)>) {
            returnVal.storage = (storage + handleDiff(rhs));
            return returnVal;
        } else {
            []<bool flag = false>() { static_assert(flag, "Wrong data type operator +"); }();
        }
    }

    //Subtraction operator overloading.
    template<typename T>
    FixedPoint operator-(const T &rhs) {
        FixedPoint returnVal;
        if constexpr(std::is_same_v<FixedPoint, T>) {
            returnVal.storage = (storage - rhs.storage);
            return returnVal;
        } else if constexpr(std::is_integral_v<T>) {
            returnVal.storage = (storage - rhs * power());
            return returnVal;
        } else if constexpr(std::is_floating_point_v<T>) {
            int64_t toFixed = floatingPoint(rhs);
            returnVal.storage = (storage - toFixed);
            return returnVal;
        } else if constexpr(std::is_member_function_pointer_v<decltype(&T::getPrecision)>) {
            returnVal.storage = (storage - handleDiff(rhs));
            return returnVal;
        } else {
            []<bool flag = false>() { static_assert(flag, "Wrong data type operator +"); }();
        }
    }

    //Division operator overlading.
    template<typename T>
    inline FixedPoint operator/(const T &rhs) {
        FixedPoint returnVal;
        if constexpr(std::is_same_v<FixedPoint, T>) {
            constexpr int64_t limit = std::numeric_limits<int64_t>::max() / 2;
            if (rhs.storage > power() && power() > limit && storage > power()) {
                returnVal.storage = (storage * (power() / 2)) / (rhs.storage / (power() / 2));
            } else if (storage > power() && power() > limit) {
                returnVal.storage = storage / (rhs.storage / power());
            } else {
                returnVal.storage = ((storage * power()) / rhs.storage);
            }
            return returnVal;
        } else if constexpr(std::is_integral_v<T>) {
            returnVal.storage = (*this / FixedPoint(rhs));
            return returnVal;
        } else if constexpr(std::is_floating_point_v<T>) {
            returnVal.storage = (*this / FixedPoint(rhs));
            return returnVal;
        } else if constexpr(std::is_member_function_pointer_v<decltype(&T::getPrecision)>) {
            returnVal.storage = (*this / handleDiff(rhs));
            return returnVal;
        } else {
            []<bool flag = false>() { static_assert(flag, "Wrong data type operator +"); }();
        }
    }

    //Multiplication operator overloading.
    template<typename T>
    FixedPoint operator*(const T &rhs) {
        FixedPoint returnVal;
        if constexpr(std::is_same_v<FixedPoint, T>) {
            constexpr int64_t limit = std::numeric_limits<int64_t>::max() / 2;
            if (rhs.storage > power() && power() > limit && storage > power()) {
                returnVal.storage = (storage / (power() / 2)) * (rhs.storage / (power() / 2));
            } else if (rhs.storage > power() && power() > limit) {
                returnVal.storage = (rhs.storage / power()) * storage;
            } else if (storage > power() && power() > limit) {
                returnVal.storage = (storage / power()) * rhs.storage;
            } else {
                int64_t temp = storage * rhs.storage;
                returnVal.storage = (temp / power());
            }
            return returnVal;
        } else if constexpr(std::is_integral_v<T>) {
            returnVal.storage = (*this * FixedPoint(rhs));
            return returnVal;
        } else if constexpr(std::is_floating_point_v<T>) {
            returnVal.storage = (*this * FixedPoint(rhs));
            return returnVal;
        } else if constexpr(std::is_member_function_pointer_v<decltype(&T::getPrecision)>) {
            returnVal.storage = (*this * handleDiff(rhs));
            return returnVal;
        } else {
            []<bool flag = false>() { static_assert(flag, "Wrong data type operator +"); }();
        }
    }

    static FixedPoint sqrt(FixedPoint t) {
        FixedPoint half(0.5);
        FixedPoint final(1);
        int max = 20;
        if (t.storage / power() > 1000000)
            max = 20;
        else if (t.storage / power() > 10000)
            max = 15;
        else
            max = 10;
        for (int i = 0; i < max; i++) {
            final = half * (final + (t / final));
        }
        return final;
    }

    static FixedPoint pow(FixedPoint t, int x) {
        FixedPoint result = 1;
        for (int i = 0; i < x; i++) {
            result = result * t;
        }
        return result;
    }

private:
    int64_t storage;
};


#endif //FIXEDPOINTMATH_FIXEDPOINT_H
