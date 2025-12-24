#pragma once

// Public umbrella header (preferred include location).
// These headers are provided via the include/ tree.

// Core
#include "core/Matrix.h"
#include "core/Vector.h"

// Linear
#include "linear/GaussianElimination.h"
#include "linear/GaussSeidel.h"
#include "linear/Jacobi.h"
#include "linear/LinearSystem.h"

// Nonlinear
#include "nonlinear/Newton.h"
#include "nonlinear/NonlinearSystem.h"
#include "nonlinear/RootFinding.h"
#include "nonlinear/ScalarEquation.h"

// Utils
#include "utils/Exceptions.h"
#include "utils/Rounding.h"
