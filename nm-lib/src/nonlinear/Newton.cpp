#include "nonlinear/Newton.h"

#include "utils/Exceptions.h"

#include <cmath>
#include <limits>
#include <stdexcept>

static bool isFiniteVector(const Vector& v)
{
    for (std::size_t i = 0; i < v.size(); i++)
    {
        if (!std::isfinite(v[i]))
        {
            return false;
        }
    }
    return true;
}

static Vector solveLinearSystemGaussianPivot(Matrix A, Vector b)
{
    const std::size_t n = b.size();
    if (A.rowCount() != n || A.colCount() != n)
    {
        throw DimensionMismatchException("NewtonSolver: Jacobian dimension mismatch");
    }

    constexpr double pivotEps = 1e-15;

    for (std::size_t k = 0; k < n; k++)
    {
        std::size_t pivotRow = k;
        double maxAbs = std::fabs(A(k, k));
        for (std::size_t i = k + 1; i < n; i++)
        {
            const double candidate = std::fabs(A(i, k));
            if (candidate > maxAbs)
            {
                maxAbs = candidate;
                pivotRow = i;
            }
        }

        if (maxAbs < pivotEps)
        {
            throw SingularMatrixException("NewtonSolver: singular Jacobian (zero pivot)");
        }

        if (pivotRow != k)
        {
            for (std::size_t j = 0; j < n; j++)
            {
                const double tmp = A(k, j);
                A(k, j) = A(pivotRow, j);
                A(pivotRow, j) = tmp;
            }

            const double tmpB = b[k];
            b[k] = b[pivotRow];
            b[pivotRow] = tmpB;
        }

        const double pivot = A(k, k);
        if (std::fabs(pivot) < pivotEps)
        {
            throw SingularMatrixException("NewtonSolver: singular Jacobian (pivot too small)");
        }

        for (std::size_t i = k + 1; i < n; i++)
        {
            const double m = A(i, k) / pivot;
            A(i, k) = 0.0;
            for (std::size_t j = k + 1; j < n; j++)
            {
                A(i, j) -= m * A(k, j);
            }
            b[i] -= m * b[k];
        }
    }

    Vector x(n);
    for (std::size_t ii = 0; ii < n; ii++)
    {
        const std::size_t i = n - 1 - ii;
        double sum = 0.0;
        for (std::size_t j = i + 1; j < n; j++)
        {
            sum += A(i, j) * x[j];
        }

        const double diag = A(i, i);
        if (std::fabs(diag) < pivotEps)
        {
            throw SingularMatrixException("NewtonSolver: singular Jacobian during back substitution");
        }

        x[i] = (b[i] - sum) / diag;
    }

    return x;
}

Vector NewtonSolver::solve(const NonlinearSystem& system, Vector x0, double eps, NewtonSystemTrace* trace)
{
    if (eps <= 0.0)
    {
        throw std::invalid_argument("NewtonSolver::solve: eps must be positive");
    }
    if (x0.size() == 0)
    {
        throw std::invalid_argument("NewtonSolver::solve: x0 must be non-empty");
    }
    if (!isFiniteVector(x0))
    {
        throw std::invalid_argument("NewtonSolver::solve: x0 contains non-finite values");
    }

    Vector x = x0;
    constexpr std::size_t maxIterations = 100;

    if (trace)
    {
        trace->steps.clear();
    }

    for (std::size_t iter = 0; iter < maxIterations; iter++)
    {
        Vector fx = system.evaluate(x);
        if (fx.size() != x.size())
        {
            throw DimensionMismatchException("NewtonSolver::solve: F(x) dimension mismatch");
        }
        if (!isFiniteVector(fx))
        {
            throw NonConvergenceException("NewtonSolver::solve: F(x) became non-finite");
        }

        const double fxNorm = fx.normInf();
        if (fxNorm <= eps)
        {
            return x;
        }

        Matrix jac = system.jacobian(x);
        if (jac.rowCount() != x.size() || jac.colCount() != x.size())
        {
            throw DimensionMismatchException("NewtonSolver::solve: J(x) dimension mismatch");
        }

        // Solve J(x) * delta = -F(x)
        Vector rhs(x.size());
        for (std::size_t i = 0; i < x.size(); i++)
        {
            rhs[i] = -fx[i];
        }

        Vector delta = solveLinearSystemGaussianPivot(jac, rhs);
        if (!isFiniteVector(delta))
        {
            throw NonConvergenceException("NewtonSolver::solve: update became non-finite");
        }

        if (trace)
        {
            trace->steps.push_back({
                iter,
                x,
                fx,
                jac,
                delta
            });
        }

        for (std::size_t i = 0; i < x.size(); i++)
        {
            x[i] += delta[i];
        }
        if (!isFiniteVector(x))
        {
            throw NonConvergenceException("NewtonSolver::solve: iterate became non-finite");
        }

        // Course-style stop: small step (absolute or relative).
        const double xNorm = x.normInf();
        const double denom = (xNorm > 1.0) ? xNorm : 1.0;
        if (delta.normInf() / denom <= eps)
        {
            return x;
        }
    }

    throw NonConvergenceException("NewtonSolver::solve: maximum iterations reached");
}
