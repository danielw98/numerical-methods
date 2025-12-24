#pragma once

#include "core/Matrix.h"

class LinearSystem {
private:
	Matrix A;
	Vector b;

public:
	LinearSystem(const Matrix& A, const Vector& b);

	std::size_t size() const;

	const Matrix& matrix() const;
	const Vector& rhs() const;
};
