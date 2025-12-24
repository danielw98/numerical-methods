#include "nonlinear/NonlinearSystem.h"

#include <utility>

NonlinearSystem::NonlinearSystem(VectorFunction F, JacobianFunction J)
    : F(std::move(F)), J(std::move(J))
{
}

Vector NonlinearSystem::evaluate(const Vector& x) const
{
    return F(x);
}

Matrix NonlinearSystem::jacobian(const Vector& x) const
{
    return J(x);
}
