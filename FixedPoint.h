#ifndef FIXEDPOINTMATH_FIXEDPOINT_H
#define FIXEDPOINTMATH_FIXEDPOINT_H

#include <bitset>
#include <cassert>
#include <cmath>
#include <iostream>

template<std::size_t decimalPoints>
class FixedPoint {
public:
    template<std::size_t decimal = decimalPoints>
    static constexpr typename std::enable_if<decimal == 0, int64_t>::type power() {
        return 1;
    }

    template<std::size_t decimal = decimalPoints>
    static constexpr typename std::enable_if<decimal != 0, int64_t>::type power() {
        return 10 * power<decimal - 1>();
    }

    constexpr std::size_t getPrecision() {
        return decimalPoints;
    }

    const int64_t getStorage() {
        return storage;
    }

    FixedPoint() {
        static_assert(decimalPoints <= 18);
        static_assert(decimalPoints != 0);
        storage = 0;
    };

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

    template<typename t>
    FixedPoint(t start) {
        static_assert(decimalPoints <= 18);
        static_assert(decimalPoints != 0);
        handler(start);
    }

    void print() {
        std::cout << (storage / power()) << "." << (storage % power()) << std::endl;
    }

    bool operator==(const FixedPoint &rhs) const {
        return storage == rhs.storage;
    }

    bool operator!=(const FixedPoint &rhs) const {
        return !(rhs == *this);
    }

    template<typename t>
    FixedPoint &operator=(const t &rhs) {
        handler(rhs);
        return *this;
    }

    FixedPoint &operator=(const std::pair<int64_t, uint64_t> majorMinor){
        assert(majorMinor.first <= (std::numeric_limits<int64_t>::max() / power()) &&
               majorMinor.first >= (std::numeric_limits<int64_t>::min() / power()) && majorMinor.second < power());
        storage = majorMinor.first * power() + majorMinor.second;
        return *this;
    }

    template<typename T>
    FixedPoint &operator+(const T &rhs) {
        if constexpr(std::is_same_v<FixedPoint, T>) {
            storage += rhs.storage;
            return *this;
        } else if constexpr(std::is_integral_v<T>) {
            storage += rhs * power();
            return *this;
        } else if constexpr(std::is_floating_point_v<T>) {
            int64_t toFixed = floatingPoint(rhs);
            storage += toFixed;
            return *this;
        } else if constexpr(std::is_member_function_pointer_v<decltype(&T::getPrecision)>) {
            storage += handleDiff(rhs);
            return *this;
        } else {
            []<bool flag = false>() { static_assert(flag, "Wrong data type operator +"); }();
        }
    }

    template<typename T>
    FixedPoint &operator-(const T &rhs) {
        if constexpr(std::is_same_v<FixedPoint, T>) {
            storage -= rhs.storage;
            return *this;
        } else if constexpr(std::is_integral_v<T>) {
            storage -= rhs * power();
            return *this;
        } else if constexpr(std::is_floating_point_v<T>) {
            int64_t toFixed = floatingPoint(rhs);
            storage -= toFixed;
            return *this;
        } else if constexpr(std::is_member_function_pointer_v<decltype(&T::getPrecision)>) {
            storage -= handleDiff(rhs);
            return *this;
        } else {
            []<bool flag = false>() { static_assert(flag, "Wrong data type operator +"); }();
        }
    }

    template<typename T>
    inline FixedPoint &operator/(const T &rhs) {
        if constexpr(std::is_same_v<FixedPoint, T>) {
            storage = (storage * power()) / rhs.storage;
            return *this;
        } else if constexpr(std::is_integral_v<T>) {
            *this / FixedPoint(rhs);
            return *this;
        } else if constexpr(std::is_floating_point_v<T>) {
            *this / FixedPoint(rhs);
            return *this;
        } else if constexpr(std::is_member_function_pointer_v<decltype(&T::getPrecision)>) {
            *this / handleDiff(rhs);
            return *this;
        } else {
            []<bool flag = false>() { static_assert(flag, "Wrong data type operator +"); }();
        }
    }

    template<typename T>
    FixedPoint &operator*(const T &rhs) {
        if constexpr(std::is_same_v<FixedPoint, T>) {
            storage = storage * rhs.storage;
            storage /= power();
            return *this;
        } else if constexpr(std::is_integral_v<T>) {
            *this * FixedPoint(rhs);
            return *this;
        } else if constexpr(std::is_floating_point_v<T>) {
            *this * FixedPoint(rhs);
            return *this;
        } else if constexpr(std::is_member_function_pointer_v<decltype(&T::getPrecision)>) {
            *this * handleDiff(rhs);
            return *this;
        } else {
            []<bool flag = false>() { static_assert(flag, "Wrong data type operator +"); }();
        }
    }

private:
    int64_t storage;
};


#endif //FIXEDPOINTMATH_FIXEDPOINT_H
