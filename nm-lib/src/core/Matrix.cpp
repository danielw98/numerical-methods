#include "core/Matrix.h"

#include <stdexcept>

Matrix::Matrix(std::size_t rows, std::size_t cols)
    : rows(rows), cols(cols), data(rows * cols, 0.0)
{
}

std::size_t Matrix::rowCount() const
{
    return rows;
}

std::size_t Matrix::colCount() const
{
    return cols;
}

double& Matrix::operator()(std::size_t i, std::size_t j)
{
    if (i >= rows || j >= cols) {
        throw std::out_of_range("Matrix index out of range");
    }
    return data[i * cols + j];
}

const double& Matrix::operator()(std::size_t i, std::size_t j) const
{
    if (i >= rows || j >= cols) {
        throw std::out_of_range("Matrix index out of range");
    }
    return data[i * cols + j];
}

Matrix Matrix::identity(std::size_t n)
{
    Matrix I(n, n);
    for (std::size_t i = 0; i < n; ++i) {
        I(i, i) = 1.0;
    }
    return I;
}

Vector Matrix::multiply(const Vector& x) const
{
    if (x.size() != cols) {
        throw std::invalid_argument("Matrix::multiply dimension mismatch");
    }

    Vector y(rows);
    for (std::size_t i = 0; i < rows; ++i) {
        double sum = 0.0;
        for (std::size_t j = 0; j < cols; ++j) {
            sum += (*this)(i, j) * x[j];
        }
        y[i] = sum;
    }

    return y;
}
