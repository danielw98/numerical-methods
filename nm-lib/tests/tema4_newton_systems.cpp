#include "NumericalMethods.h"
#include "utils/JsonUtils.h"

#include "nlohmann/json.hpp"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <cctype>

using nlohmann::json;

static void printVector(const Vector& x)
{
    std::cout << "[";
    for (std::size_t i = 0; i < x.size(); i++)
    {
        if (i)
        {
            std::cout << ", ";
        }
        std::cout << x[i];
    }
    std::cout << "]\n";
}

static NonlinearSystem system1()
{
    // Referat 02 — Tema 2 — (1)
    // f1 = x1 + 2 x2^2 - x2 - 2 x3 = 0
    // f2 = x2 - 8 x2^2 + 10 x3 = 0
    // f3 = x2 (7 x2 x3)^{-1} - 1 = 0  => (for x2 != 0) 1/(7 x3) - 1 = 0
    VectorFunction F = [](const Vector& x) {
        Vector fx(3);
        const double x1 = x[0];
        const double x2 = x[1];
        const double x3 = x[2];

        fx[0] = x1 + 2.0 * x2 * x2 - x2 - 2.0 * x3;
        fx[1] = x2 - 8.0 * x2 * x2 + 10.0 * x3;
        fx[2] = (1.0 / (7.0 * x3)) - 1.0;
        return fx;
    };

    JacobianFunction J = [](const Vector& x) {
        Matrix jac(3, 3);
        const double x2 = x[1];
        const double x3 = x[2];

        jac(0, 0) = 1.0;
        jac(0, 1) = 4.0 * x2 - 1.0;
        jac(0, 2) = -2.0;

        jac(1, 0) = 0.0;
        jac(1, 1) = 1.0 - 16.0 * x2;
        jac(1, 2) = 10.0;

        jac(2, 0) = 0.0;
        jac(2, 1) = 0.0;
        jac(2, 2) = -(1.0 / (7.0 * x3 * x3));

        return jac;
    };

    return NonlinearSystem(F, J);
}

static NonlinearSystem system2()
{
    // (2)
    // f1 = x1^2 + x2 - 37 = 0
    // f2 = x1 - x2^2 - 5 = 0
    // f3 = x1 + x2 + x3 - 3 = 0
    VectorFunction F = [](const Vector& x) {
        Vector fx(3);
        const double x1 = x[0];
        const double x2 = x[1];
        const double x3 = x[2];
        fx[0] = x1 * x1 + x2 - 37.0;
        fx[1] = x1 - x2 * x2 - 5.0;
        fx[2] = x1 + x2 + x3 - 3.0;
        return fx;
    };

    JacobianFunction J = [](const Vector& x) {
        Matrix jac(3, 3);
        const double x1 = x[0];
        const double x2 = x[1];

        jac(0, 0) = 2.0 * x1;
        jac(0, 1) = 1.0;
        jac(0, 2) = 0.0;

        jac(1, 0) = 1.0;
        jac(1, 1) = -2.0 * x2;
        jac(1, 2) = 0.0;

        jac(2, 0) = 1.0;
        jac(2, 1) = 1.0;
        jac(2, 2) = 1.0;

        return jac;
    };

    return NonlinearSystem(F, J);
}

static NonlinearSystem system3()
{
    // (3)
    // f1 = x1^2 + x2^2 - x1 = 0
    // f2 = x1^2 - x2^2 - x2 = 0
    VectorFunction F = [](const Vector& x) {
        Vector fx(2);
        const double x1 = x[0];
        const double x2 = x[1];
        fx[0] = x1 * x1 + x2 * x2 - x1;
        fx[1] = x1 * x1 - x2 * x2 - x2;
        return fx;
    };

    JacobianFunction J = [](const Vector& x) {
        Matrix jac(2, 2);
        const double x1 = x[0];
        const double x2 = x[1];

        jac(0, 0) = 2.0 * x1 - 1.0;
        jac(0, 1) = 2.0 * x2;

        jac(1, 0) = 2.0 * x1;
        jac(1, 1) = -2.0 * x2 - 1.0;

        return jac;
    };

    return NonlinearSystem(F, J);
}

static NonlinearSystem system4()
{
    // (4)
    // f1 = 3x1^2 - x2^2 = 0
    // f2 = 3x1 x2^2 - x1^3 - 1 = 0
    VectorFunction F = [](const Vector& x) {
        Vector fx(2);
        const double x1 = x[0];
        const double x2 = x[1];
        fx[0] = 3.0 * x1 * x1 - x2 * x2;
        fx[1] = 3.0 * x1 * x2 * x2 - x1 * x1 * x1 - 1.0;
        return fx;
    };

    JacobianFunction J = [](const Vector& x) {
        Matrix jac(2, 2);
        const double x1 = x[0];
        const double x2 = x[1];

        jac(0, 0) = 6.0 * x1;
        jac(0, 1) = -2.0 * x2;

        jac(1, 0) = 3.0 * x2 * x2 - 3.0 * x1 * x1;
        jac(1, 1) = 6.0 * x1 * x2;

        return jac;
    };

    return NonlinearSystem(F, J);
}

int main(int argc, char** argv)
{
    // Usage: tema4_newton_systems [--json] [--trace|--no-trace] <system:1|2|3|4>
    bool jsonMode = false;
    bool traceMode = false;
    int argi = 1;

    auto normFlag = [](const char* s) {
        std::string f = s ? std::string(s) : std::string();
        while (!f.empty() && std::isspace(static_cast<unsigned char>(f.back())))
        {
            f.pop_back();
        }
        while (!f.empty() && std::isspace(static_cast<unsigned char>(f.front())))
        {
            f.erase(f.begin());
        }
        for (char& c : f)
        {
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }
        return f;
    };

    while (argi < argc)
    {
        const std::string flag = normFlag(argv[argi]);
        if (flag == "--json" || flag == "-json")
        {
            jsonMode = true;
            argi++;
            continue;
        }
        if (flag == "--trace" || flag == "-trace")
        {
            traceMode = true;
            argi++;
            continue;
        }
        if (flag == "--no-trace" || flag == "-no-trace")
        {
            traceMode = false;
            argi++;
            continue;
        }
        break;
    }

    int which = 4;
    for (int i = argi; i < argc; i++)
    {
        char* end = nullptr;
        const long parsed = std::strtol(argv[i], &end, 10);
        if (end != argv[i] && end != nullptr && *end == '\0')
        {
            which = static_cast<int>(parsed);
            break;
        }
    }

    if (which < 1 || which > 4)
    {
        std::cerr << "Invalid system. Use 1, 2, 3, or 4.\n";
        return 2;
    }

    NonlinearSystem sys = system1();
    Vector x0(3);
    std::string statement;

    if (which == 1)
    {
        sys = system1();
        x0 = Vector{ 0.3, 0.5, 0.14 };
        statement = "x1 + 2x2^2 - x2 - 2x3 = 0;  x2 - 8x2^2 + 10x3 = 0;  1/(7x3) - 1 = 0";
    }
    else if (which == 2)
    {
        sys = system2();
        x0 = Vector{ 6.0, 6.0, -9.0 };
        statement = "x1^2 + x2 - 37 = 0;  x1 - x2^2 - 5 = 0;  x1 + x2 + x3 - 3 = 0";
    }
    else if (which == 3)
    {
        sys = system3();
        x0 = Vector{ 0.5, 0.5 };
        statement = "x1^2 + x2^2 - x1 = 0;  x1^2 - x2^2 - x2 = 0";
    }
    else if (which == 4)
    {
        sys = system4();
        x0 = Vector{ 1.0, 2.0 };
        statement = "3x1^2 - x2^2 = 0;  3x1 x2^2 - x1^3 - 1 = 0";
    }
    else
    {
        std::cerr << "Invalid system. Use 1, 2, 3, or 4.\n";
        return 2;
    }

    const double eps = 1e-5;
    NewtonSystemTrace trace;
    Vector x = NewtonSolver::solve(sys, x0, eps, traceMode ? &trace : nullptr);

    if (jsonMode)
    {
        const Vector fx = sys.evaluate(x);

        auto traceToJson = [&](const NewtonSystemTrace& t) {
            json arr = json::array();
            for (const auto& s : t.steps)
            {
                arr.push_back(json{
                    { "iter", s.iter },
                    { "x", vectorToJson(s.x) },
                    { "fx", vectorToJson(s.fx) },
                    { "jac", matrixToJson(s.jac) },
                    { "delta", vectorToJson(s.delta) }
                });
            }
            return arr;
        };

        json j;
        j["kind"] = "newton_systems";
        j["referat"] = "02";
        j["system"] = which;
        j["eps"] = eps;
        j["statement"] = statement;
        j["x0"] = vectorToJson(x0);
        j["x"] = vectorToJson(x);
        j["fx"] = vectorToJson(fx);
        j["residual_inf"] = fx.normInf();

        j["traceEnabled"] = traceMode;
        if (traceMode)
        {
            j["trace"] = {
                { "iterations", traceToJson(trace) }
            };
        }
        std::cout << j.dump(2) << "\n";
        return 0;
    }

    std::cout << "Solution x = ";
    printVector(x);
    return 0;
}
