#ifndef FIXEDPOINTMATH_FIXEDPOINT_H
#define FIXEDPOINTMATH_FIXEDPOINT_H

#include <bitset>
#include <assert.h>
#include <cmath>
#include <iostream>

template<std::size_t decimalPoints>
class FixedPoint {
public:
    constexpr int64_t power() {
        return std::pow(10, decimalPoints);
    }

    FixedPoint() {
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
    FixedPoint(t start) {
        storage = 0;
        if constexpr(std::is_floating_point_v<t>){
            storage = floatingPoint(start);
        }else if constexpr(std::is_same_v<FixedPoint, t>){
            storage = start.storage;
            return;
        }else if constexpr(std::is_integral_v<t>){
            assert(start <= (std::numeric_limits<int64_t>::max() / power()) &&
                   start >= (std::numeric_limits<int64_t>::min() / power()));

            storage = (int64_t) (std::pow(10, decimalPoints)) * start;
        }else{
            []<bool flag = false>()
            {static_assert(flag, "Wrong data type");}();
        }
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

    FixedPoint& operator=(const FixedPoint& rhs){
        storage = rhs.storage;
        return *this;
    }

    template<typename T>
    FixedPoint& operator+(const T& rhs){
        if constexpr(std::is_same_v<FixedPoint, T>){
            storage += rhs.storage;
            return *this;
        }else if constexpr(std::is_integral_v<T>){
            storage += rhs * power();
            return *this;
        }else if constexpr(std::is_floating_point_v<T>){
            int64_t toFixed = floatingPoint(rhs);
            storage += toFixed;
            return *this;
        }else{
            []<bool flag = false>()
            {static_assert(flag, "Wrong data type operator +");}();
        }
    }

    template<typename T>
    FixedPoint& operator-(const T& rhs){
        if constexpr(std::is_same_v<FixedPoint, T>){
            storage -= rhs.storage;
            return *this;
        }else if constexpr(std::is_integral_v<T>){
            storage -= rhs * power();
            return *this;
        }else if constexpr(std::is_floating_point_v<T>){
            int64_t toFixed = floatingPoint(rhs);
            storage -= toFixed;
            return *this;
        }else{
            []<bool flag = false>()
            {static_assert(flag, "Wrong data type operator +");}();
        }
    }

    template<typename T>
    inline FixedPoint& operator/(const T& rhs){
        if constexpr(std::is_same_v<FixedPoint, T>){
            storage = (storage * power()) / rhs.storage;
            return *this;
        }else if constexpr(std::is_integral_v<T>){
            *this / FixedPoint(rhs);
            return *this;
        }else if constexpr(std::is_floating_point_v<T>){
            *this / FixedPoint(rhs);
            return *this;
        }else{
            []<bool flag = false>()
            {static_assert(flag, "Wrong data type operator +");}();
        }
    }

    template<typename T>
    FixedPoint& operator*(const T& rhs){
        if constexpr(std::is_same_v<FixedPoint, T>){
            storage = storage * rhs.storage;
            storage /= power();
            return *this;
        }else if constexpr(std::is_integral_v<T>){
            *this * FixedPoint(rhs);
            return *this;
        }else if constexpr(std::is_floating_point_v<T>){
            *this * FixedPoint(rhs);
            return *this;
        }else{
            []<bool flag = false>()
            {static_assert(flag, "Wrong data type operator +");}();
        }
    }

private:
    int64_t storage;
};


#endif //FIXEDPOINTMATH_FIXEDPOINT_H
