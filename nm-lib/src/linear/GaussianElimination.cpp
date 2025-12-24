#include "linear/GaussianElimination.h"

#include "utils/Exceptions.h"
#include "utils/Rounding.h"

#include <cmath>
#include <iomanip>
#include <sstream>
#include <stdexcept>

Vector GaussianElimination::solve(LinearSystem system, int significantDigits, GaussianEliminationTrace* trace)
{
    if (significantDigits <= 0)
    {
        throw std::invalid_argument("significantDigits must be positive");
    }

    const std::size_t n = system.size();
    const Matrix& Aref = system.matrix();
    const Vector& bref = system.rhs();

    if (Aref.rowCount() != n || Aref.colCount() != n || bref.size() != n)
    {
        throw DimensionMismatchException("GaussianElimination::solve: dimension mismatch");
    }

    auto r = [&](double v)
    {
        return roundToSignificantDigits(v, significantDigits);
    };

    Matrix A = Aref;
    Vector b = bref;

    auto pushOp = [&](const std::string& phase, const std::string& op, bool hasSolveValue = false, std::size_t solveIndex = 0, double solveValue = 0.0)
    {
        if (!trace)
        {
            return;
        }

        trace->operations.push_back({
            phase,
            op,
            A,
            b,
            hasSolveValue,
            solveIndex,
            solveValue
        });
    };

    auto fmt = [&](double v) -> std::string
    {
        std::ostringstream oss;
        oss.setf(std::ios::fmtflags(0), std::ios::floatfield);
        oss << std::setprecision(8) << v;
        return oss.str();
    };

    if (trace)
    {
        trace->operations.clear();
        trace->forwardSteps.clear();
        pushOp("forward", "Matrice augmentată inițială");
    }

    // Forward elimination with partial pivoting.
    constexpr double pivotEps = 1e-15;
    for (std::size_t k = 0; k < n; k++)
    {
        // Choose pivot row.
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
            throw SingularMatrixException("GaussianElimination::solve: singular matrix (zero pivot)");
        }

        {
            const std::string op =
                "Aleg pivot pe coloana " + std::to_string(k + 1) +
                ": R" + std::to_string(pivotRow + 1);
            pushOp("forward", op);
        }

        // Swap rows if needed.
        bool swapped = false;
        if (pivotRow != k)
        {
            swapped = true;
            for (std::size_t j = 0; j < n; j++)
            {
                const double tmp = A(k, j);
                A(k, j) = A(pivotRow, j);
                A(pivotRow, j) = tmp;
            }
            const double tmpB = b[k];
            b[k] = b[pivotRow];
            b[pivotRow] = tmpB;

            {
                const std::string op =
                    "Schimb linii: R" + std::to_string(k + 1) +
                    " <-> R" + std::to_string(pivotRow + 1);
                pushOp("forward", op);
            }
        }

        const double pivot = A(k, k);
        if (std::fabs(pivot) < pivotEps)
        {
            throw SingularMatrixException("GaussianElimination::solve: singular matrix (zero pivot after swap)");
        }

        for (std::size_t i = k + 1; i < n; i++)
        {
            const double multiplier = r(A(i, k) / pivot);

            // A(i,k) becomes 0 by construction.
            A(i, k) = 0.0;

            for (std::size_t j = k + 1; j < n; j++)
            {
                const double product = r(multiplier * A(k, j));
                A(i, j) = r(A(i, j) - product);
            }

            const double productB = r(multiplier * b[k]);
            b[i] = r(b[i] - productB);

            {
                const std::string op =
                    "R" + std::to_string(i + 1) +
                    " <- R" + std::to_string(i + 1) +
                    " - (" + fmt(multiplier) + ") * R" + std::to_string(k + 1);
                pushOp("forward", op);
            }
        }

        if (trace)
        {
            trace->forwardSteps.push_back({
                k,
                pivotRow,
                swapped,
                A,
                b
            });
        }
    }

    pushOp("forward", "Matrice superior triunghiulară (după eliminare)");

    // Back substitution.
    Vector x(n);
    for (std::size_t ii = 0; ii < n; ii++)
    {
        const std::size_t i = n - 1 - ii;

        double sum = 0.0;
        for (std::size_t j = i + 1; j < n; j++)
        {
            sum = r(sum + r(A(i, j) * x[j]));
        }

        const double diag = A(i, i);
        if (std::fabs(diag) < pivotEps)
        {
            throw SingularMatrixException("GaussianElimination::solve: singular matrix (zero diagonal)");
        }

        const double rhs = r(b[i] - sum);
        x[i] = r(rhs / diag);

        {
            const std::string op =
                "Substituție înapoi: x" + std::to_string(i + 1) +
                " = " + fmt(x[i]);
            pushOp("back", op, true, i, x[i]);
        }
    }

    pushOp("back", "Soluție finală (după substituție înapoi)");

    return x;
}
