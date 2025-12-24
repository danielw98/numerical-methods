#pragma once

#include <vector>

class Vector {
private:
	std::vector<double> data;

public:
	explicit Vector(std::size_t n);
	Vector(std::initializer_list<double> init);

	std::size_t size() const;

	double& operator[](std::size_t i);
	const double& operator[](std::size_t i) const;

	double normInf() const;
};
