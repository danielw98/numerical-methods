#include "linear/Jacobi.h"

#include "utils/Exceptions.h"

#include <cmath>
#include <stdexcept>

Vector JacobiSolver::iterate(const LinearSystem& system, const Vector& x0, std::size_t iterations, IterativeMethodTrace* trace)
{
    const std::size_t n = system.size();
    const Matrix& A = system.matrix();
    const Vector& b = system.rhs();

    if (A.rowCount() != n || A.colCount() != n || b.size() != n)
    {
        throw DimensionMismatchException("JacobiSolver::iterate: dimension mismatch");
    }
    if (x0.size() != n)
    {
        throw DimensionMismatchException("JacobiSolver::iterate: x0 dimension mismatch");
    }

    Vector xPrev = x0;
    Vector xNext(n);

    if (trace)
    {
        trace->steps.clear();
        trace->steps.push_back({ 0, xPrev });
    }

    constexpr double diagEps = 1e-15;
    for (std::size_t it = 0; it < iterations; it++)
    {
        for (std::size_t i = 0; i < n; i++)
        {
            const double aii = A(i, i);
            if (std::fabs(aii) < diagEps)
            {
                throw SingularMatrixException("JacobiSolver::iterate: zero diagonal entry");
            }

            double sum = 0.0;
            for (std::size_t j = 0; j < n; j++)
            {
                if (j == i)
                {
                    continue;
                }
                sum += A(i, j) * xPrev[j];
            }

            xNext[i] = (b[i] - sum) / aii;
        }

        xPrev = xNext;

        if (trace)
        {
            trace->steps.push_back({ it + 1, xPrev });
        }
    }

    return xPrev;
}
