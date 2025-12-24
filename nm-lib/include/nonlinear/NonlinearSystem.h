#pragma once

#include "core/Matrix.h"
#include <functional>

using VectorFunction = std::function<Vector(const Vector &)>;
using JacobianFunction = std::function<Matrix(const Vector &)>;

class NonlinearSystem {
private:
	VectorFunction F;
	JacobianFunction J;

public:
	NonlinearSystem(VectorFunction F, JacobianFunction J);

	Vector evaluate(const Vector &x) const;
	Matrix jacobian(const Vector &x) const;
};
