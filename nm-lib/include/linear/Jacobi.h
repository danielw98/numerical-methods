#pragma once

#include "linear/LinearSystem.h"

#include "linear/IterativeTrace.h"

class JacobiSolver {
public:
	JacobiSolver() = delete;    
    
	static Vector iterate(const LinearSystem& system, const Vector& x0, std::size_t iterations, IterativeMethodTrace* trace = nullptr);
};
