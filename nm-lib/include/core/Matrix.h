#pragma once

#include "core/Vector.h"

class Matrix {
private:
	std::size_t rows;
	std::size_t cols;
	std::vector<double> data; // row-major

public:
	Matrix(std::size_t rows, std::size_t cols);

	std::size_t rowCount() const;
	std::size_t colCount() const;

	double &operator()(std::size_t i, std::size_t j);
	const double &operator()(std::size_t i, std::size_t j) const;

	static Matrix identity(std::size_t n);

	Vector multiply(const Vector &x) const;
};
