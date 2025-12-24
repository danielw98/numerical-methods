#pragma once

#include "nonlinear/ScalarEquation.h"

#include <cstddef>
#include <vector>

struct BisectionTraceStep
{
	std::size_t iter;
	double a;
	double b;
	double p;
	double fp;
	double errorBound;
};

struct BisectionTrace
{
	std::vector<BisectionTraceStep> steps;
};

struct RegulaFalsiTraceStep
{
	std::size_t iter;
	double a;
	double b;
	double p;
	double fp;
};

struct RegulaFalsiTrace
{
	std::vector<RegulaFalsiTraceStep> steps;
};

struct SecantTraceStep
{
	std::size_t iter;
	double x0;
	double x1;
	double p;
	double fp;
};

struct SecantTrace
{
	std::vector<SecantTraceStep> steps;
};

struct NewtonTraceStep
{
	std::size_t iter;
	double x;
	double fx;
	double dfx;
	double xNext;
	double fxNext;
};

struct NewtonTrace
{
	std::vector<NewtonTraceStep> steps;
};

class RootFinding {
public:
	RootFinding() = delete;
    
	static double bisection(const ScalarEquation& eq, double a, double b, double eps, BisectionTrace* trace = nullptr);

	static double regulaFalsi(const ScalarEquation& eq, double a, double b, double eps, RegulaFalsiTrace* trace = nullptr);

	static double secant(const ScalarEquation& eq, double x0, double x1, double eps, SecantTrace* trace = nullptr);

	static double newton(const ScalarEquation& eq, Function1D derivative, double x0, double eps, NewtonTrace* trace = nullptr);
};
