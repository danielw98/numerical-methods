#include "NumericalMethods.h"

#include "utils/JsonUtils.h"

#include "nlohmann/json.hpp"

#include <cmath>
#include <iostream>
#include <string>
#include <cctype>

using nlohmann::json;

static double residualInf(const Matrix& A, const Vector& x, const Vector& b)
{
    const std::size_t n = b.size();
    double maxAbs = 0.0;
    for (std::size_t i = 0; i < n; i++)
    {
        double s = 0.0;
        for (std::size_t j = 0; j < n; j++)
        {
            s += A(i, j) * x[j];
        }
        const double ri = s - b[i];
        const double ai = std::fabs(ri);
        if (ai > maxAbs)
        {
            maxAbs = ai;
        }
    }
    return maxAbs;
}

static LinearSystem system1()
{
    Matrix A(3, 3);
    Vector b(3);

    A(0, 0) = 3.03;
    A(0, 1) = -12.1;
    A(0, 2) = 14.0;
    b[0] = -119.0;

    A(1, 0) = -3.03;
    A(1, 1) = 12.1;
    A(1, 2) = -7.0;
    b[1] = 120.0;

    A(2, 0) = 6.11;
    A(2, 1) = -14.2;
    A(2, 2) = 21.0;
    b[2] = -139.0;

    return LinearSystem(A, b);
}

static LinearSystem system2()
{
    Matrix A(3, 3);
    Vector b(3);

    A(0, 0) = 3.333;
    A(0, 1) = 15920.0;
    A(0, 2) = 10.333;
    b[0] = 7953.0;

    A(1, 0) = 2.222;
    A(1, 1) = 16.71;
    A(1, 2) = 9.612;
    b[1] = 0.965;

    A(2, 0) = -1.5611;
    A(2, 1) = 5.1792;
    A(2, 2) = -1.6855;
    b[2] = 2714.0;

    return LinearSystem(A, b);
}

static LinearSystem system3()
{
    Matrix A(4, 4);
    Vector b(4);

    A(0, 0) = 2.12;
    A(0, 1) = -2.12;
    A(0, 2) = 51.3;
    A(0, 3) = 100.0;
    b[0] = 3.14159265358979323846;

    A(1, 0) = 0.333;
    A(1, 1) = -0.333;
    A(1, 2) = -12.2;
    A(1, 3) = 19.7;
    b[1] = std::sqrt(2.0);

    A(2, 0) = 6.19;
    A(2, 1) = 8.20;
    A(2, 2) = -1.0;
    A(2, 3) = -2.01;
    b[2] = 0.0;

    A(3, 0) = -5.73;
    A(3, 1) = 6.12;
    A(3, 2) = 1.0;
    A(3, 3) = -1.0;
    b[3] = -1.0;

    return LinearSystem(A, b);
}

static LinearSystem system4()
{
    Matrix A(4, 4);
    Vector b(4);

    const double pi = 3.14159265358979323846;
    A(0, 0) = pi;
    A(0, 1) = std::sqrt(2.0);
    A(0, 2) = -1.0;
    A(0, 3) = 1.0;
    b[0] = 0.0;

    A(1, 0) = std::exp(1.0);
    A(1, 1) = -1.0;
    A(1, 2) = 1.0;
    A(1, 3) = 2.0;
    b[1] = 1.0;

    A(2, 0) = 1.0;
    A(2, 1) = 1.0;
    A(2, 2) = -std::sqrt(3.0);
    A(2, 3) = 1.0;
    b[2] = 2.0;

    A(3, 0) = -1.0;
    A(3, 1) = -1.0;
    A(3, 2) = 1.0;
    A(3, 3) = -std::sqrt(5.0);
    b[3] = 3.0;

    return LinearSystem(A, b);
}

static void printVector(const Vector& x)
{
    std::cout << "[";
    for (std::size_t i = 0; i < x.size(); ++i) {
        if (i) {
            std::cout << ", ";
        }
        std::cout << x[i];
    }
    std::cout << "]\n";
}

int main(int argc, char** argv)
{
    // Usage: tema2_gauss [--json] [--trace|--no-trace] <system:1-4>
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

    int which = 1;
    if (argc >= argi + 1)
    {
        which = std::stoi(argv[argi]);
    }

    LinearSystem sys = system1();
    switch (which) {
    case 1:
        sys = system1();
        break;
    case 2:
        sys = system2();
        break;
    case 3:
        sys = system3();
        break;
    case 4:
        sys = system4();
        break;
    default:
        std::cerr << "Invalid system. Use 1-4.\n";
        return 2;
    }

    const int significantDigits = 3;
    GaussianEliminationTrace trace;
    Vector x = GaussianElimination::solve(sys, significantDigits, traceMode ? &trace : nullptr);

    if (!jsonMode)
    {
        std::cout << "Solution x = ";
        printVector(x);
        return 0;
    }

    json j;
    j["kind"] = "gauss";
    j["referat"] = "01";
    j["system"] = which;
    j["significantDigits"] = significantDigits;
    j["A"] = matrixToJson(sys.matrix());
    j["b"] = vectorToJson(sys.rhs());
    j["x"] = vectorToJson(x);
    j["residual_inf"] = residualInf(sys.matrix(), x, sys.rhs());

    j["traceEnabled"] = traceMode;
    if (traceMode)
    {
        json steps = json::array();
        for (const auto& s : trace.forwardSteps)
        {
            steps.push_back({
                { "k", s.k },
                { "pivotRow", s.pivotRow },
                { "swapped", s.swapped },
                { "A", matrixToJson(s.A) },
                { "b", vectorToJson(s.b) }
            });
        }

        json ops = json::array();
        for (const auto& op : trace.operations)
        {
            ops.push_back({
                { "phase", op.phase },
                { "op", op.op },
                { "A", matrixToJson(op.A) },
                { "b", vectorToJson(op.b) },
                { "hasSolveValue", op.hasSolveValue },
                { "solveIndex", op.solveIndex },
                { "solveValue", op.solveValue }
            });
        }

        j["trace"] = {
            { "forwardElimination", steps },
            { "operations", ops }
        };
    }

    std::cout << j.dump(2) << "\n";
    return 0;
}
