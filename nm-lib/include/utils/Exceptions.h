#pragma once

#include <stdexcept>

class NumericalException : public std::runtime_error {
public:
	using std::runtime_error::runtime_error;
};

class DimensionMismatchException : public NumericalException {
public:
	using NumericalException::NumericalException;
};

class SingularMatrixException : public NumericalException {
public:
	using NumericalException::NumericalException;
};

class NonConvergenceException : public NumericalException {
public:
	using NumericalException::NumericalException;
};
