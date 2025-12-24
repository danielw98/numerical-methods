#include "NumericalMethods.h"

#include "utils/JsonUtils.h"

#include "nlohmann/json.hpp"

#include <iostream>
#include <cmath>
#include <string>
#include <cctype>

using nlohmann::json;

static Vector zeros(std::size_t n)
{
    return Vector(n);
}

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

static LinearSystem system1()
{
    // 5x5
    Matrix A(5, 5);
    Vector b(5);

    A(0, 0) = 4;  A(0, 1) = 1;  A(0, 2) = 1;  A(0, 3) = 0;  A(0, 4) = 1;  b[0] = 6;
    A(1, 0) = -1; A(1, 1) = -3; A(1, 2) = 1;  A(1, 3) = 1;  A(1, 4) = 0;  b[1] = 6;
    A(2, 0) = 2;  A(2, 1) = 1;  A(2, 2) = 5;  A(2, 3) = -1; A(2, 4) = -1; b[2] = 6;
    A(3, 0) = -1; A(3, 1) = -1; A(3, 2) = -1; A(3, 3) = 4;  A(3, 4) = 0;  b[3] = 6;
    A(4, 0) = 0;  A(4, 1) = 2;  A(4, 2) = -1; A(4, 3) = 1;  A(4, 4) = 4;  b[4] = 6;

    return LinearSystem(A, b);
}

static LinearSystem system2()
{
    // 6x6
    Matrix A(6, 6);
    Vector b(6);

    A(0, 0) = 4;  A(0, 1) = -1; A(0, 2) = 0;  A(0, 3) = -1; A(0, 4) = 0;  A(0, 5) = 0;  b[0] = 0;
    A(1, 0) = -1; A(1, 1) = 4;  A(1, 2) = -1; A(1, 3) = 0;  A(1, 4) = -1; A(1, 5) = 0;  b[1] = 5;
    A(2, 0) = 0;  A(2, 1) = -1; A(2, 2) = 4;  A(2, 3) = 0;  A(2, 4) = 0;  A(2, 5) = -1; b[2] = 0;
    A(3, 0) = -1; A(3, 1) = 0;  A(3, 2) = 0;  A(3, 3) = 4;  A(3, 4) = -1; A(3, 5) = 0;  b[3] = 6;
    A(4, 0) = 0;  A(4, 1) = -1; A(4, 2) = 0;  A(4, 3) = -1; A(4, 4) = 4;  A(4, 5) = -1; b[4] = -2;
    A(5, 0) = 0;  A(5, 1) = 0;  A(5, 2) = -1; A(5, 3) = 0;  A(5, 4) = -1; A(5, 5) = 4;  b[5] = 6;

    return LinearSystem(A, b);
}

static LinearSystem system3()
{
    // 4x4 (tridiagonal-ish)
    Matrix A(4, 4);
    Vector b(4);

    A(0, 0) = 10; A(0, 1) = 5;  A(0, 2) = 0;  A(0, 3) = 0;  b[0] = 6;
    A(1, 0) = 5;  A(1, 1) = 10; A(1, 2) = -4; A(1, 3) = 0;  b[1] = 25;
    A(2, 0) = 0;  A(2, 1) = -4; A(2, 2) = 8;  A(2, 3) = -1; b[2] = -11;
    A(3, 0) = 0;  A(3, 1) = 0;  A(3, 2) = -1; A(3, 3) = 5;  b[3] = -11;

    return LinearSystem(A, b);
}

static LinearSystem system4()
{
    // 4x4
    Matrix A(4, 4);
    Vector b(4);

    A(0, 0) = 4;  A(0, 1) = 1;  A(0, 2) = -1; A(0, 3) = 1;  b[0] = -2;
    A(1, 0) = 1;  A(1, 1) = 4;  A(1, 2) = -1; A(1, 3) = -1; b[1] = -1;
    A(2, 0) = -1; A(2, 1) = -1; A(2, 2) = 5;  A(2, 3) = 1;  b[2] = 0;
    A(3, 0) = 1;  A(3, 1) = -1; A(3, 2) = 1;  A(3, 3) = 3;  b[3] = 1;

    return LinearSystem(A, b);
}

int main(int argc, char** argv)
{
    // Usage: tema3_iterative [--json] [--trace|--no-trace] <system:1-4>
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

    const std::size_t iterations = 2;
    Vector x0 = zeros(sys.size());

    IterativeMethodTrace jacobiTrace;
    IterativeMethodTrace gsTrace;

    const Vector xJacobi = JacobiSolver::iterate(sys, x0, iterations, traceMode ? &jacobiTrace : nullptr);
    const Vector xGs = GaussSeidelSolver::iterate(sys, x0, iterations, traceMode ? &gsTrace : nullptr);

    if (jsonMode)
    {
        auto traceToJson = [&](const IterativeMethodTrace& t) {
            json arr = json::array();
            for (const auto& s : t.steps)
            {
                arr.push_back({
                    { "iter", s.iter },
                    { "x", vectorToJson(s.x) }
                });
            }
            return arr;
        };

        json j;
        j["kind"] = "iterative";
        j["referat"] = "01";
        j["system"] = which;
        j["iterations"] = iterations;
        j["A"] = matrixToJson(sys.matrix());
        j["b"] = vectorToJson(sys.rhs());
        j["x0"] = vectorToJson(x0);

        j["traceEnabled"] = traceMode;

        j["methods"] = json::array();
        json mJac = {
            { "name", "jacobi" },
            { "x", vectorToJson(xJacobi) },
            { "residual_inf", residualInf(sys.matrix(), xJacobi, sys.rhs()) }
        };
        if (traceMode)
        {
            mJac["trace"] = traceToJson(jacobiTrace);
        }
        j["methods"].push_back(mJac);

        json mGs = {
            { "name", "gaussSeidel" },
            { "x", vectorToJson(xGs) },
            { "residual_inf", residualInf(sys.matrix(), xGs, sys.rhs()) }
        };
        if (traceMode)
        {
            mGs["trace"] = traceToJson(gsTrace);
        }
        j["methods"].push_back(mGs);

        std::cout << j.dump(2) << "\n";
        return 0;
    }

    std::cout << "Jacobi x^(2) = ";
    printVector(xJacobi);

    std::cout << "Gauss-Seidel x^(2) = ";
    printVector(xGs);

    return 0;
}
