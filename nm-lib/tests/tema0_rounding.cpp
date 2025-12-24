#include "NumericalMethods.h"

#include <cmath>
#include <cstdlib>
#include <iostream>

static bool nearlyEqual(double a, double b, double relTol = 1e-12, double absTol = 1e-12)
{
    const double diff = std::fabs(a - b);
    if (diff <= absTol) {
        return true;
    }
    return diff <= relTol * std::max(std::fabs(a), std::fabs(b));
}

static void expectEqual(const char* name, double got, double expected)
{
    if (!nearlyEqual(got, expected)) {
        std::cerr << "FAIL: " << name << ": got " << got << ", expected " << expected << "\n";
        std::exit(1);
    }
    std::cout << "OK: " << name << "\n";
}

int main()
{
    // Round-half-to-even tie cases (first removed digit is 5 followed only by zeros).
    expectEqual("1.25 -> 2 sig digits", roundToSignificantDigits(1.25, 2), 1.2);
    expectEqual("1.35 -> 2 sig digits", roundToSignificantDigits(1.35, 2), 1.4);
    expectEqual("1250 -> 2 sig digits", roundToSignificantDigits(1250.0, 2), 1200.0);
    expectEqual("1350 -> 2 sig digits", roundToSignificantDigits(1350.0, 2), 1400.0);

    // Non-tie examples.
    expectEqual("0.012349 -> 3 sig digits", roundToSignificantDigits(0.012349, 3), 0.0123);
    expectEqual("0.012351 -> 3 sig digits", roundToSignificantDigits(0.012351, 3), 0.0124);

    // Sign handling.
    expectEqual("-1.25 -> 2 sig digits", roundToSignificantDigits(-1.25, 2), -1.2);

    std::cout << "All rounding checks passed.\n";
    return 0;
}
