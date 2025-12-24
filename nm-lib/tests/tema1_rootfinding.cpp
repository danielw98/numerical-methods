#include "NumericalMethods.h"

#include "nlohmann/json.hpp"

#include <cmath>
#include <iostream>
#include <string>
#include <cctype>
#include <optional>

using nlohmann::json;

static void runEquation1(bool secondInterval)
{
    // f(x) = x^2 - 4x + 4 - ln(x)
    ScalarEquation eq([](double x) { return x * x - 4.0 * x + 4.0 - std::log(x); });
    Function1D der = [](double x) { return 2.0 * x - 4.0 - 1.0 / x; };

    const double eps = 1e-7;
    const double a = secondInterval ? 2.0 : 1.0;
    const double b = secondInterval ? 4.0 : 2.0;

    std::cout << "Eq(1), interval [" << a << ", " << b << "]\n";
    std::cout << "  bisection    = " << RootFinding::bisection(eq, a, b, eps) << "\n";
    std::cout << "  regulaFalsi  = " << RootFinding::regulaFalsi(eq, a, b, eps) << "\n";
    std::cout << "  secant       = " << RootFinding::secant(eq, a, b, eps) << "\n";
    std::cout << "  newton       = " << RootFinding::newton(eq, der, (a + b) / 2.0, eps) << "\n";
}

static void runEquation2(bool secondInterval)
{
    // f(x) = x + 1 - 2 sin(pi x)
    constexpr double pi = 3.14159265358979323846;
    ScalarEquation eq([&](double x) { return x + 1.0 - 2.0 * std::sin(pi * x); });
    Function1D der([&](double x) { return 1.0 - 2.0 * pi * std::cos(pi * x); });

    const double eps = 1e-7;
    const double a = secondInterval ? 0.5 : 0.0;
    const double b = secondInterval ? 1.0 : 0.5;

    std::cout << "Eq(2), interval [" << a << ", " << b << "]\n";
    std::cout << "  bisection    = " << RootFinding::bisection(eq, a, b, eps) << "\n";
    std::cout << "  regulaFalsi  = " << RootFinding::regulaFalsi(eq, a, b, eps) << "\n";
    std::cout << "  secant       = " << RootFinding::secant(eq, a, b, eps) << "\n";
    std::cout << "  newton       = " << RootFinding::newton(eq, der, (a + b) / 2.0, eps) << "\n";
}

static void runEquation3(bool secondInterval)
{
    // f(x) = e^x - 3x^2
    ScalarEquation eq([](double x) { return std::exp(x) - 3.0 * x * x; });
    Function1D der([](double x) { return std::exp(x) - 6.0 * x; });

    const double eps = 1e-7;
    const double a = secondInterval ? 3.0 : 0.0;
    const double b = secondInterval ? 5.0 : 1.0;

    std::cout << "Eq(3), interval [" << a << ", " << b << "]\n";
    std::cout << "  bisection    = " << RootFinding::bisection(eq, a, b, eps) << "\n";
    std::cout << "  regulaFalsi  = " << RootFinding::regulaFalsi(eq, a, b, eps) << "\n";
    std::cout << "  secant       = " << RootFinding::secant(eq, a, b, eps) << "\n";
    std::cout << "  newton       = " << RootFinding::newton(eq, der, (a + b) / 2.0, eps) << "\n";
}

static void runEquation4(bool secondInterval)
{
    // f(x) = 2x cos(2x) - (x - 2)^2
    ScalarEquation eq([](double x) { return 2.0 * x * std::cos(2.0 * x) - (x - 2.0) * (x - 2.0); });
    Function1D der([](double x) {
        // derivative: 2cos(2x) - 4x sin(2x) - 2(x-2)
        return 2.0 * std::cos(2.0 * x) - 4.0 * x * std::sin(2.0 * x) - 2.0 * (x - 2.0);
    });

    const double eps = 1e-7;
    const double a = secondInterval ? 3.0 : 2.0;
    const double b = secondInterval ? 4.0 : 3.0;

    std::cout << "Eq(4), interval [" << a << ", " << b << "]\n";
    std::cout << "  bisection    = " << RootFinding::bisection(eq, a, b, eps) << "\n";
    std::cout << "  regulaFalsi  = " << RootFinding::regulaFalsi(eq, a, b, eps) << "\n";
    std::cout << "  secant       = " << RootFinding::secant(eq, a, b, eps) << "\n";
    std::cout << "  newton       = " << RootFinding::newton(eq, der, (a + b) / 2.0, eps) << "\n";
}

int main(int argc, char** argv)
{
    // Usage:
    //   tema1_rootfinding [--json] [--trace|--no-trace] <eq:1-4> <interval:1|2>
    //   tema1_rootfinding [--json] [--trace|--no-trace] --eq <1-4> [--interval <1|2>] [--a <num> --b <num>] [--x0 <num>] [--eps <num>]
    bool jsonMode = false;
    bool traceMode = false;
    int argi = 1;

    std::optional<int> eqFlag;
    std::optional<int> intervalFlag;
    std::optional<double> aFlag;
    std::optional<double> bFlag;
    std::optional<double> x0Flag;
    std::optional<double> epsFlag;

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

    auto parseDoubleFlagValue = [&](int& i) -> std::optional<double> {
        if (i + 1 >= argc)
        {
            return std::nullopt;
        }
        try
        {
            return std::stod(argv[i + 1]);
        }
        catch (...)
        {
            return std::nullopt;
        }
    };

    auto parseIntFlagValue = [&](int& i) -> std::optional<int> {
        if (i + 1 >= argc)
        {
            return std::nullopt;
        }
        try
        {
            return std::stoi(argv[i + 1]);
        }
        catch (...)
        {
            return std::nullopt;
        }
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
        if (flag == "--eq")
        {
            const auto v = parseIntFlagValue(argi);
            if (!v)
            {
                std::cerr << "Missing/invalid value for --eq\n";
                return 2;
            }
            eqFlag = *v;
            argi += 2;
            continue;
        }
        if (flag == "--interval")
        {
            const auto v = parseIntFlagValue(argi);
            if (!v)
            {
                std::cerr << "Missing/invalid value for --interval\n";
                return 2;
            }
            intervalFlag = *v;
            argi += 2;
            continue;
        }
        if (flag == "--a")
        {
            const auto v = parseDoubleFlagValue(argi);
            if (!v)
            {
                std::cerr << "Missing/invalid value for --a\n";
                return 2;
            }
            aFlag = *v;
            argi += 2;
            continue;
        }
        if (flag == "--b")
        {
            const auto v = parseDoubleFlagValue(argi);
            if (!v)
            {
                std::cerr << "Missing/invalid value for --b\n";
                return 2;
            }
            bFlag = *v;
            argi += 2;
            continue;
        }
        if (flag == "--x0")
        {
            const auto v = parseDoubleFlagValue(argi);
            if (!v)
            {
                std::cerr << "Missing/invalid value for --x0\n";
                return 2;
            }
            x0Flag = *v;
            argi += 2;
            continue;
        }
        if (flag == "--eps")
        {
            const auto v = parseDoubleFlagValue(argi);
            if (!v)
            {
                std::cerr << "Missing/invalid value for --eps\n";
                return 2;
            }
            epsFlag = *v;
            argi += 2;
            continue;
        }
        break;
    }

    int eq = eqFlag.value_or(1);
    int interval = intervalFlag.value_or(1);

    // Backward-compatible positional args when --eq/--interval aren't provided.
    if (!eqFlag && argc >= argi + 1)
    {
        eq = std::stoi(argv[argi]);
    }
    if (!intervalFlag && argc >= argi + 2)
    {
        interval = std::stoi(argv[argi + 1]);
    }

    const bool secondInterval = (interval == 2);

    if (!jsonMode)
    {
        switch (eq) {
    case 1:
        runEquation1(secondInterval);
        break;
    case 2:
        runEquation2(secondInterval);
        break;
    case 3:
        runEquation3(secondInterval);
        break;
    case 4:
        runEquation4(secondInterval);
        break;
    default:
        std::cerr << "Invalid equation. Use 1-4.\n";
        return 2;
        }

        return 0;
    }

    // JSON mode: emit a stable schema for the frontend.
    double eps = epsFlag.value_or(1e-7);
    double a = 0.0;
    double b = 0.0;
    std::string statement;

    ScalarEquation equation([](double) { return 0.0; });
    Function1D derivative([](double) { return 0.0; });
    double x0 = 0.0;

    if (eq == 1)
    {
        statement = "x^2 - 4x + 4 - ln(x) = 0";
        equation = ScalarEquation([](double x) { return x * x - 4.0 * x + 4.0 - std::log(x); });
        derivative = [](double x) { return 2.0 * x - 4.0 - 1.0 / x; };
        a = secondInterval ? 2.0 : 1.0;
        b = secondInterval ? 4.0 : 2.0;
    }
    else if (eq == 2)
    {
        constexpr double pi = 3.14159265358979323846;
        statement = "x + 1 - 2 sin(pi x) = 0";
        equation = ScalarEquation([&](double x) { return x + 1.0 - 2.0 * std::sin(pi * x); });
        derivative = Function1D([&](double x) { return 1.0 - 2.0 * pi * std::cos(pi * x); });
        a = secondInterval ? 0.5 : 0.0;
        b = secondInterval ? 1.0 : 0.5;
    }
    else if (eq == 3)
    {
        statement = "e^x - 3x^2 = 0";
        equation = ScalarEquation([](double x) { return std::exp(x) - 3.0 * x * x; });
        derivative = [](double x) { return std::exp(x) - 6.0 * x; };
        a = secondInterval ? 3.0 : 0.0;
        b = secondInterval ? 5.0 : 1.0;
    }
    else if (eq == 4)
    {
        statement = "2x cos(2x) - (x-2)^2 = 0";
        equation = ScalarEquation([](double x) { return 2.0 * x * std::cos(2.0 * x) - (x - 2.0) * (x - 2.0); });
        derivative = [](double x) {
            return 2.0 * std::cos(2.0 * x) - 4.0 * x * std::sin(2.0 * x) - 2.0 * (x - 2.0);
        };
        a = secondInterval ? 3.0 : 2.0;
        b = secondInterval ? 4.0 : 3.0;
    }
    else
    {
        std::cerr << "Invalid equation. Use 1-4.\n";
        return 2;
    }

    // Optional overrides from CLI.
    if (aFlag) a = *aFlag;
    if (bFlag) b = *bFlag;
    if (!(a < b))
    {
        std::cerr << "Invalid interval. Require a < b.\n";
        return 2;
    }
    if (eq == 1 && (a <= 0.0 || b <= 0.0))
    {
        std::cerr << "Invalid interval for eq(1). Require a>0 and b>0 because ln(x).\n";
        return 2;
    }

    x0 = x0Flag.value_or((a + b) / 2.0);

    BisectionTrace bisTrace;
    RegulaFalsiTrace rfTrace;
    SecantTrace secTrace;
    NewtonTrace newtTrace;

    const double xBis = RootFinding::bisection(equation, a, b, eps, traceMode ? &bisTrace : nullptr);
    const double xRf = RootFinding::regulaFalsi(equation, a, b, eps, traceMode ? &rfTrace : nullptr);
    const double xSec = RootFinding::secant(equation, a, b, eps, traceMode ? &secTrace : nullptr);
    const double xNew = RootFinding::newton(equation, derivative, x0, eps, traceMode ? &newtTrace : nullptr);

    auto bisTraceToJson = [&](const BisectionTrace& t) {
        json arr = json::array();
        for (const auto& s : t.steps)
        {
            arr.push_back({
                { "iter", s.iter },
                { "a", s.a },
                { "b", s.b },
                { "p", s.p },
                { "fp", s.fp },
                { "errorBound", s.errorBound }
            });
        }
        return arr;
    };

    auto rfTraceToJson = [&](const RegulaFalsiTrace& t) {
        json arr = json::array();
        for (const auto& s : t.steps)
        {
            arr.push_back({
                { "iter", s.iter },
                { "a", s.a },
                { "b", s.b },
                { "p", s.p },
                { "fp", s.fp }
            });
        }
        return arr;
    };

    auto secTraceToJson = [&](const SecantTrace& t) {
        json arr = json::array();
        for (const auto& s : t.steps)
        {
            arr.push_back({
                { "iter", s.iter },
                { "x0", s.x0 },
                { "x1", s.x1 },
                { "p", s.p },
                { "fp", s.fp }
            });
        }
        return arr;
    };

    auto newtTraceToJson = [&](const NewtonTrace& t) {
        json arr = json::array();
        for (const auto& s : t.steps)
        {
            arr.push_back({
                { "iter", s.iter },
                { "x", s.x },
                { "fx", s.fx },
                { "dfx", s.dfx },
                { "xNext", s.xNext },
                { "fxNext", s.fxNext }
            });
        }
        return arr;
    };

    json j;
    j["kind"] = "rootfinding";
    j["referat"] = "01";
    j["eq"] = eq;
    j["interval"] = interval;
    j["eps"] = eps;
    j["statement"] = statement;
    j["a"] = a;
    j["b"] = b;
    j["traceEnabled"] = traceMode;
    j["x0"] = x0;
    j["methods"] = json::array();

    json mBis = { { "name", "bisection" }, { "x", xBis }, { "fx", equation(xBis) } };
    if (traceMode) mBis["trace"] = bisTraceToJson(bisTrace);
    j["methods"].push_back(mBis);

    json mRf = { { "name", "regulaFalsi" }, { "x", xRf }, { "fx", equation(xRf) } };
    if (traceMode) mRf["trace"] = rfTraceToJson(rfTrace);
    j["methods"].push_back(mRf);

    json mSec = { { "name", "secant" }, { "x", xSec }, { "fx", equation(xSec) } };
    if (traceMode) mSec["trace"] = secTraceToJson(secTrace);
    j["methods"].push_back(mSec);

    json mNew = { { "name", "newton" }, { "x", xNew }, { "fx", equation(xNew) } };
    if (traceMode) mNew["trace"] = newtTraceToJson(newtTrace);
    j["methods"].push_back(mNew);

    std::cout << j.dump(2) << "\n";

    return 0;
}
