#pragma once


#include <iostream>
#include <map>
#include <tuple>

struct Index {
    size_t x{0};
    size_t y{0};

    Index() = default;
    
    // for fancy view only
    Index& with__x(const size_t& x_) {
        this->x = x_; 
        return *this;
    }
    Index& with__y(const size_t& y_) {
        this->y = y_; 
        return *this;
    }

    bool operator<(const Index& other) const {
        return std::tie(x, y) < std::tie(other.x, other.y);
    }
};

template<typename Type, Type DefaultValue>
class Matrix {
    std::map<Index, Type> data;

public:
    class CellProxy {
        Matrix& matrix;
        size_t x, y;
    public:
        CellProxy(Matrix& m, size_t row, size_t col) : matrix(m), x(row), y(col) {}

        operator Type() const {
            auto it = matrix.data.find({x, y});
            return (it != matrix.data.end()) ? it->second : DefaultValue;
        }

        CellProxy& operator=(const Type& value) {
            if (value == DefaultValue) {
                matrix.data.erase({x, y});
            } else {
                matrix.data[{x, y}] = value;
            }
            return *this;
        }

        CellProxy& operator=(const CellProxy& other) {
            return *this = static_cast<Type>(other);
        }
    };

    class RowProxy {
        Matrix& matrix;
        size_t x;
    public:
        RowProxy(Matrix& m, size_t row) : matrix(m), x(row) {}
        CellProxy operator[](size_t y) {
            return CellProxy(matrix, x, y);
        }
    };

    RowProxy operator[](size_t x) {
        return RowProxy(*this, x);
    }

    size_t size() const { return data.size(); }


    struct Iterator {
        using InternalIt = typename std::map<Index, Type>::iterator;
        InternalIt it;

        bool operator!=(const Iterator& other) const { return it != other.it; }
        void operator++() { ++it; }
        auto operator*() { return std::make_tuple(it->first.x, it->first.y, it->second); }
    };

    Iterator begin() { return {data.begin()}; }
    Iterator end() { return {data.end()}; }
};
