#include "linear/GaussSeidel.h"

#include "utils/Exceptions.h"

#include <cmath>
#include <stdexcept>

Vector GaussSeidelSolver::iterate(const LinearSystem& system, const Vector& x0, std::size_t iterations, IterativeMethodTrace* trace)
{
    const std::size_t n = system.size();
    const Matrix& A = system.matrix();
    const Vector& b = system.rhs();

    if (A.rowCount() != n || A.colCount() != n || b.size() != n)
    {
        throw DimensionMismatchException("GaussSeidelSolver::iterate: dimension mismatch");
    }
    if (x0.size() != n)
    {
        throw DimensionMismatchException("GaussSeidelSolver::iterate: x0 dimension mismatch");
    }

    Vector x = x0;
    constexpr double diagEps = 1e-15;

    if (trace)
    {
        trace->steps.clear();
        trace->steps.push_back({ 0, x });
    }

    for (std::size_t it = 0; it < iterations; it++)
    {
        for (std::size_t i = 0; i < n; i++)
        {
            const double aii = A(i, i);
            if (std::fabs(aii) < diagEps)
            {
                throw SingularMatrixException("GaussSeidelSolver::iterate: zero diagonal entry");
            }

            double sum = 0.0;
            for (std::size_t j = 0; j < n; j++)
            {
                if (j == i)
                {
                    continue;
                }
                sum += A(i, j) * x[j];
            }

            x[i] = (b[i] - sum) / aii;
        }

        if (trace)
        {
            trace->steps.push_back({ it + 1, x });
        }
    }

    return x;
}
