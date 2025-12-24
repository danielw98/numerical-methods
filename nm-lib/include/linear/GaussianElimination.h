#pragma once

#include "linear/LinearSystem.h"

#include <cstddef>
#include <string>
#include <vector>

struct GaussianEliminationTraceStep
{
	std::size_t k;
	std::size_t pivotRow;
	bool swapped;
	Matrix A;
	Vector b;
};

struct GaussianEliminationTrace
{
	std::vector<GaussianEliminationTraceStep> forwardSteps;

	// Fine-grained trace, suitable for a step-by-step slideshow:
	// - forward: pivot choice, row swaps, and each row operation
	// - back: each back-substitution step that computes x_i
	struct OperationStep
	{
		std::string phase; // "forward" | "back"
		std::string op;    // human-readable operation label
		Matrix A;
		Vector b;
		bool hasSolveValue;
		std::size_t solveIndex; // 0-based index for x
		double solveValue;
	};

	std::vector<OperationStep> operations;
};

class GaussianElimination {
public:
	GaussianElimination() = delete;

	static Vector solve(LinearSystem system, int significantDigits, GaussianEliminationTrace* trace = nullptr);
};
