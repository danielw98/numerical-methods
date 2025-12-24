#include "utils/Rounding.h"

#include <cmath>
#include <limits>
#include <stdexcept>

static long double roundHalfToEven(long double x)
{
    if (!std::isfinite(x))
    {
        return x;
    }

    const long double integerPart = std::floor(x);
    const long double fractionalPart = x - integerPart;

    constexpr long double eps = 1e-12L;

    if (fractionalPart > 0.5L + eps)
    {
        return integerPart + 1.0L;
    }
    if (fractionalPart < 0.5L - eps)
    {
        return integerPart;
    }

    // Tie case (first removed digit is 5 followed only by zeros): round to even.
    const long double integerPartAsEvenCheck = std::fmod(integerPart, 2.0L);
    const bool isEven = std::fabs(integerPartAsEvenCheck) < eps;
    return isEven ? integerPart : (integerPart + 1.0L);
}

double roundToSignificantDigits(double value, int significantDigits)
{
    if (significantDigits <= 0)
    {
        throw std::invalid_argument("significantDigits must be positive");
    }

    if (!std::isfinite(value) || value == 0.0)
    {
        return value;
    }

    const long double absValue = std::fabs(static_cast<long double>(value));
    const long double sign = (value < 0.0) ? -1.0L : 1.0L;

    const long double exponent = std::floor(std::log10(absValue));
    const long double scalePower = (significantDigits - 1) - exponent;
    const long double scale = std::pow(10.0L, scalePower);

    // Guard against overflow/underflow in scaling.
    if (!std::isfinite(scale) || scale == 0.0L)
    {
        return value;
    }

    const long double scaled = absValue * scale;
    const long double roundedScaled = roundHalfToEven(scaled);
    const long double result = (roundedScaled / scale) * sign;

    return result;
}
