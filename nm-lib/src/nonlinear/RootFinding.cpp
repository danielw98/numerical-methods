#include "nonlinear/RootFinding.h"

#include "utils/Exceptions.h"

#include <cmath>
#include <limits>
#include <stdexcept>

static int signum(double x)
{
    if (x > 0.0)
    {
        return 1;
    }
    if (x < 0.0)
    {
        return -1;
    }
    return 0;
}

static void validateEps(double eps)
{
    if (!(eps > 0.0))
    {
        throw std::invalid_argument("eps must be > 0");
    }
}

static void validateBracket(const ScalarEquation& eq, double a, double b)
{
    if (!(a < b))
    {
        throw std::invalid_argument("invalid interval: require a < b");
    }

    const double fa = eq(a);
    const double fb = eq(b);

    if (std::isnan(fa) || std::isnan(fb) || std::isinf(fa) || std::isinf(fb))
    {
        throw std::invalid_argument("f(a) or f(b) is not finite");
    }

    const int sa = signum(fa);
    const int sb = signum(fb);
    if (sa == sb)
    {
        throw std::invalid_argument("interval does not bracket a root (same sign at endpoints)");
    }
}

double RootFinding::bisection(const ScalarEquation& eq, double a, double b, double eps, BisectionTrace* trace)
{
    validateEps(eps);
    validateBracket(eq, a, b);

    double fa = eq(a);
    double fb = eq(b);

    if (std::abs(fa) < eps) {
        return a;
    }
    if (std::abs(fb) < eps) {
        return b;
    }

    const std::size_t maxIterations = 1000;
    for (std::size_t iter = 0; iter < maxIterations; iter++)
    {
        // pick midpoint and avoid ovrflow from a+b
        const double p = a + (b - a) / 2.0;
        // evaluate function in midpoint
        const double fp = eq(p);

        // when tracing we save the iteration, endpoints, midpoint
        // function value at midpoint and the maximum error at this step
        if (trace) {
            trace->steps.push_back({ iter, a, b, p, fp, std::abs(b - a) / 2.0 });
        }

        // found solution within tolerance
        if (std::abs(fp) <= eps) {
            return p;
        }
        if (std::abs(b - a) / 2.0 <= eps) {
            return p;
        }

        // sign of the function at midpoint
        const int sp = signum(fp);
        if (sp == 0) {
            return p;
        }

        // change the interval into [a, p] or [p,b] depending on signs
        const int sa = signum(fa);
        const int sb = signum(fb);
        if (sa == sp) {
            a = p;
            fa = fp;
        }
        if (sb == sp) {
            b = p;
            fb = fp;
        }
    }
    // reached max iterations and no solution was found
    throw NonConvergenceException("bisection did not converge within iteration limit");
}

double RootFinding::regulaFalsi(const ScalarEquation& eq, double a, double b, double eps, RegulaFalsiTrace* trace)
{
    validateEps(eps);
    // regula falsi (false position) is a bracketing method:
    // - we start with an interval [a,b] such that f(a) and f(b) have opposite signs
    // - we approximate the root by intersecting the secant through (a,f(a)) and (b,f(b)) with the x-axis
    //   p = (a*f(b) - b*f(a)) / (f(b) - f(a))
    // - we keep the root bracketed by replacing the endpoint that has the same sign as f(p)
    validateBracket(eq, a, b);

    double fa = eq(a);
    double fb = eq(b);

    if (fa == 0.0)
    {
        return a;
    }
    if (fb == 0.0)
    {
        return b;
    }

    double prevP = std::numeric_limits<double>::quiet_NaN();
    const std::size_t maxIterations = 100000;
    for (std::size_t iter = 0; iter < maxIterations; iter++)
    {
        // compute the secant intersection with OX for the current bracket [a,b]
        // (the denominator must be non-zero to define the secant slope)
        const double denom = (fb - fa);
        if (denom == 0.0) {
            throw std::invalid_argument("regula falsi failed: f(b) - f(a) == 0");
        }

        // regula falsi update (false position)
        // p is where the secant line crosses y=0
        const double p = (a * fb - b * fa) / denom;
        // evaluate function in the candidate point
        const double fp = eq(p);

        // when tracing we save the iteration, endpoints and candidate
        if (trace) {
            trace->steps.push_back({ iter, a, b, p, fp });
        }

        // stop criteria: small residual / small change / small bracket
        if (std::abs(fp) <= eps) {
            return p;
        }
        // stop if successive approximations stop moving (common practical criterion)
        if (std::isfinite(prevP) && std::abs(p - prevP) <= eps) {
            return p;
        }
        // stop if the bracket is very small (bracket-based criterion)
        if (std::abs(b - a) <= 2.0 * eps) {
            return p;
        }

        // decide which side to keep so the root stays bracketed
        // (keep opposite signs at endpoints)
        const int sp = signum(fp);
        if (sp == 0) {
            return p;
        }

        const int sa = signum(fa);
        if (sa == sp) {
            a = p;
            fa = fp;
        }
        else
        {
            b = p;
            fb = fp;
        }

        // store previous approximation for the next step
        prevP = p;
    }

    throw NonConvergenceException("regula falsi did not converge within iteration limit");
}

double RootFinding::secant(const ScalarEquation& eq, double x0, double x1, double eps, SecantTrace* trace)
{
    validateEps(eps);

    // secant method is a 2-point open method:
    // - unlike bisection/regula falsi, it does not require a bracket
    // - it uses the last two iterates (x0,f(x0)) and (x1,f(x1))
    // - update formula (teacher style):
    //   p = x1 - f(x1) * (x1 - x0) / (f(x1) - f(x0))
    // - typical stop criteria: small residual |f(p)| or small step |p-x1|

    double f0 = eq(x0);
    double f1 = eq(x1);

    if (!std::isfinite(f0) || !std::isfinite(f1))
    {
        throw std::invalid_argument("secant requires finite function values at initial points");
    }
    if (std::abs(f0) <= eps)
    {
        return x0;
    }
    if (std::abs(f1) <= eps)
    {
        return x1;
    }

    const std::size_t maxIterations = 100000;
    for (std::size_t iter = 0; iter < maxIterations; iter++)
    {
        // compute secant slope denominator
        // (must be non-zero to avoid division by zero)
        const double denom = (f1 - f0);
        if (denom == 0.0) {
            throw std::invalid_argument("secant failed: f(x1) - f(x0) == 0");
        }

        // secant update using last two points
        // p is the intersection of the secant through the two points with the x-axis
        const double p = x1 - f1 * (x1 - x0) / denom;
        // evaluate function in the new approximation
        const double fp = eq(p);

        // when tracing we save the iteration and the two previous points
        if (trace) {
            trace->steps.push_back({ iter, x0, x1, p, fp });
        }

        if (!std::isfinite(fp)) {
            throw std::invalid_argument("secant produced non-finite f(p)");
        }
        // stop criteria: small residual or small step
        if (std::abs(fp) <= eps) {
            return p;
        }
        // stop if iterates stop moving
        if (std::abs(p - x1) <= eps) {
            return p;
        }

        // advance the two-point window for the next iteration
        x0 = x1;
        f0 = f1;
        x1 = p;
        f1 = fp;
    }

    throw NonConvergenceException("secant did not converge within iteration limit");
}

double RootFinding::newton(const ScalarEquation& eq, Function1D derivative, double x0, double eps, NewtonTrace* trace)
{
    validateEps(eps);

    // newton method (tangent method) is an open method:
    // - requires a derivative function f'(x)
    // - update formula (teacher style):
    //   p = x - f(x)/f'(x)
    // - convergence is typically fast near a simple root, but it is not guaranteed
    // - we must explicitly guard against f'(x)=0 and non-finite values
    if (!derivative)
    {
        throw std::invalid_argument("newton requires a valid derivative function");
    }

    double f0 = eq(x0);
    if (!std::isfinite(f0))
    {
        throw std::invalid_argument("newton requires finite f(x0)");
    }
    if (std::abs(f0) <= eps)
    {
        return x0;
    }

    const std::size_t maxIterations = 1000;
    for (std::size_t iter = 0; iter < maxIterations; iter++)
    {
        // evaluate derivative at current point
        const double df = derivative(x0);
        if (!std::isfinite(df)) {
            throw std::invalid_argument("newton requires finite f'(x)");
        }
        if (df == 0.0) {
            throw NonConvergenceException("newton failed: derivative is zero");
        }

        // newton update p = x - f(x)/f'(x)
        // p is the x-intersection of the tangent line at (x0, f(x0))
        const double p = x0 - f0 / df;
        // evaluate function in the new approximation
        const double fp = eq(p);

        // when tracing we save x, f(x), f'(x) and the next iterate
        if (trace) {
            trace->steps.push_back({ iter, x0, f0, df, p, fp });
        }

        if (!std::isfinite(fp)) {
            throw std::invalid_argument("newton produced non-finite f(p)");
        }
        // stop criteria: small residual or small step
        if (std::abs(fp) <= eps) {
            return p;
        }
        // stop if iterates stop moving
        if (std::abs(p - x0) <= eps) {
            return p;
        }

        // advance to next iterate
        x0 = p;
        f0 = fp;
    }

    throw NonConvergenceException("newton did not converge within iteration limit");
}
