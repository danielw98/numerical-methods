#pragma once

#include "nonlinear/NonlinearSystem.h"

#include <cstddef>
#include <vector>

struct NewtonSystemTraceStep
{
	std::size_t iter;
	Vector x;
	Vector fx;
	Matrix jac;
	Vector delta;
};

struct NewtonSystemTrace
{
	std::vector<NewtonSystemTraceStep> steps;
};

class NewtonSolver {
public:
	NewtonSolver() = delete;
	static Vector solve(const NonlinearSystem &system, Vector x0, double eps, NewtonSystemTrace* trace = nullptr);
};
