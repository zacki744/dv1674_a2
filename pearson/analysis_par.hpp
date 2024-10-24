#if !defined(ANALYSIS_HPP)
#define ANALYSIS_HPP

#include "vector.hpp"
#include <vector>
#include <cstddef>
namespace Analysis {
std::vector<double> correlation_coefficients(std::vector<Vector> datasets, size_t num_threads);
double pearson(Vector vec1, Vector vec2);
};

#endif
