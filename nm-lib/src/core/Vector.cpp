#include "core/Vector.h"

#include <cmath>        // std::abs
#include <stdexcept>    // std::out_of_range

Vector::Vector(std::size_t n)
    : data(n, 0.0)
{
}

Vector::Vector(std::initializer_list<double> init)
    : data(init)
{
}

std::size_t Vector::size() const
{
    return data.size();
}

double& Vector::operator[](std::size_t i)
{
    return data.at(i);
}

const double& Vector::operator[](std::size_t i) const
{
    return data.at(i);
}

double Vector::normInf() const
{
    double maxAbs = 0.0;
    for (double value : data)
    {
        const double absValue = std::abs(value);
        if (absValue > maxAbs)
        {
            maxAbs = absValue;
        }
    }
    return maxAbs;
}