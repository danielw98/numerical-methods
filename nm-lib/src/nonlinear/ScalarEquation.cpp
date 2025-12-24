#include "nonlinear/ScalarEquation.h"

#include <utility>

ScalarEquation::ScalarEquation(Function1D f)
    : f(std::move(f))
{
}

double ScalarEquation::operator()(double x) const
{
    return f(x);
}
