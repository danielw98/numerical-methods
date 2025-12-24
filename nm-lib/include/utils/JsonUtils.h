#pragma once

#include "core/Matrix.h"
#include "core/Vector.h"

#include "nlohmann/json.hpp"

#include <cstddef>

using nlohmann::json;

inline json vectorToJson(const Vector& v)
{
    json arr = json::array();
    for (std::size_t i = 0; i < v.size(); i++)
    {
        arr.push_back(v[i]);
    }
    return arr;
}

inline json matrixToJson(const Matrix& A)
{
    json rows = json::array();
    for (std::size_t i = 0; i < A.rowCount(); i++)
    {
        json row = json::array();
        for (std::size_t j = 0; j < A.colCount(); j++)
        {
            row.push_back(A(i, j));
        }
        rows.push_back(row);
    }
    return rows;
}