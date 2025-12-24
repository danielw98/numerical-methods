#pragma once

#include <functional>

using Function1D = std::function<double(double)>;

class ScalarEquation {
private:
	Function1D f;

public:
	explicit ScalarEquation(Function1D f);

	double operator()(double x) const;
};
