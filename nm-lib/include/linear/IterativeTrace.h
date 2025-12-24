#pragma once

#include "core/Vector.h"

#include <cstddef>
#include <vector>

struct IterativeTraceStep
{
    std::size_t iter;
    Vector x;
};

struct IterativeMethodTrace
{
    std::vector<IterativeTraceStep> steps;
};