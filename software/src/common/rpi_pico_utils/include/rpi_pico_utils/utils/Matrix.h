#pragma once

#include <cstdint>
#include <iostream>

template<uint32_t Rows, uint32_t Cols>
class Matrix
{
public:

    static_assert(Rows > 0 && Cols > 0);

    static Matrix<Rows, Cols> zero()
    {
        return Matrix<Rows, Cols>();
    }

    static Matrix<Rows, Cols> identity()
    {
        static_assert(Rows == Cols);
        Matrix<Rows, Cols> res;
        for(uint32_t i = 0; i < Rows; i++)
        {
            res.at(i, i) = 1.0f;
        }
        return res;
    }

    Matrix<Rows, Cols> operator*(const float& val)
    {
        Matrix<Rows, Cols> res = *this;
        for(uint32_t row = 0; row < Rows; row++)
        {
            for(uint32_t col = 0; col < Cols; col++)
            {
                res.at(row, col) *= val;
            }
        }
        return res;
    }

    Matrix<Rows, Cols> operator/(const float& val)
    {
        Matrix<Rows, Cols> res = *this;
        for(uint32_t row = 0; row < Rows; row++)
        {
            for(uint32_t col = 0; col < Cols; col++)
            {
                res.at(row, col) /= val;
            }
        }
        return res;
    }

    Matrix<Rows, Cols> operator+(const Matrix<Rows, Cols>& other)
    {
        Matrix<Rows, Cols> res = *this;
        for(uint32_t row = 0; row < Rows; row++)
        {
            for(uint32_t col = 0; col < Cols; col++)
            {
                res.at(row, col) += other.at(row, col);
            }
        }
        return res;
    }

    Matrix<Rows, Cols> operator-(const Matrix<Rows, Cols>& other)
    {
        Matrix<Rows, Cols> res = *this;
        for(uint32_t row = 0; row < Rows; row++)
        {
            for(uint32_t col = 0; col < Cols; col++)
            {
                res.at(row, col) -= other.at(row, col);
            }
        }
        return res;
    }

    Matrix<Cols, Rows> transpose()
    {
        Matrix<Cols, Rows> res = Matrix<Cols, Rows>::zero();
        for(uint32_t row = 0; row < Rows; row++)
        {
            for(uint32_t col = 0; col < Cols; col++)
            {
                res.at(col, row) = at(row, col);
            }
        }
        return res;
    }

    template<uint32_t Row, uint32_t Col>
    float& at()
    {
        static_assert(Row < Rows && Col < Cols);
        return _data[Col + Row * Cols];
    }

    template<uint32_t Row, uint32_t Col>
    const float& at() const
    {
        static_assert(Row < Rows && Col < Cols);
        return _data[Col + Row * Cols];
    }

    float& at(uint32_t row, uint32_t col)
    {
        return _data[col + row * Cols];
    }

    const float& at(uint32_t row, uint32_t col) const
    {
        return _data[col + row * Cols];
    }

    template<uint32_t OtherRows, uint32_t OtherCols>
    Matrix<Rows,OtherCols> mult(const Matrix<OtherRows, OtherCols>& other)
    {
        static_assert(OtherRows == Cols);

        Matrix<Rows,OtherCols> res = Matrix<Rows,OtherCols>::zero();
        for(uint32_t row = 0; row < Rows; row++)
        {
            for(uint32_t col = 0; col < OtherCols; col++)
            {
                float val = 0;
                for(uint32_t i = 0; i < Cols; i++)
                {
                    val += at(row, i) * other.at(i, col);
                }
                res.at(row, col) = val;
            }
        }

        return res;
    }

    void print()
    {
        for(uint32_t row = 0; row < Rows; row++)
        {
            for(uint32_t col = 0; col < Cols; col++)
            {
                std::cout << at(row, col) << ", ";
            }
            std::cout << "\n";
        }
    }

private:

    Matrix(){}

    float _data[Rows * Cols] = {0};
};