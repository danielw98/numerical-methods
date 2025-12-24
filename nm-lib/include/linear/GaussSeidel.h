#pragma once

#include "linear/LinearSystem.h"

#include "linear/IterativeTrace.h"

class GaussSeidelSolver {
public:
	GaussSeidelSolver() = delete;
    
	static Vector iterate(const LinearSystem& system, const Vector& x0, std::size_t iterations, IterativeMethodTrace* trace = nullptr);
};
