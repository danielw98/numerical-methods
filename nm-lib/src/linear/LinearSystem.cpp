#include "linear/LinearSystem.h"

LinearSystem::LinearSystem(const Matrix& A, const Vector& b)
    : A(A), b(b)
{
}

std::size_t LinearSystem::size() const
{
    return b.size();
}

const Matrix& LinearSystem::matrix() const
{
    return A;
}

const Vector& LinearSystem::rhs() const
{
    return b;
}
