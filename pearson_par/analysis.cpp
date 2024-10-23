#include "analysis.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <list>
#include <vector>
#include <thread>

namespace Analysis {

std::vector<double> correlation_coefficients(std::vector<Vector> datasets, size_t threads)
{
    std::vector<double> result(datasets.size() * (datasets.size() - 1) / 2);
    size_t index = 0;
    std::vector<std::thread> thread_pool;

    auto compute_correlation = [&](size_t start, size_t end) {
        for (size_t sample1 = start; sample1 < end; ++sample1) {
            for (size_t sample2 = sample1 + 1; sample2 < datasets.size(); ++sample2) {
                auto corr = pearson(datasets[sample1], datasets[sample2]);
                result[index++] = corr;
            }
        }
    };

    size_t chunk_size = datasets.size() / threads;
    for (size_t i = 0; i < threads; ++i) {
        size_t start = i * chunk_size;
        size_t end = (i == threads - 1) ? datasets.size() : start + chunk_size;
        thread_pool.emplace_back(compute_correlation, start, end);
    }

    for (auto& th : thread_pool) {
        if (th.joinable()) {
            th.join();
        }
    }

    return result;
}

double pearson(Vector vec1, Vector vec2)
{
    auto x_mean = vec1.mean();
    auto y_mean = vec2.mean();

    auto x_mm = vec1 - x_mean;
    auto y_mm = vec2 - y_mean;

    auto x_mag = x_mm.magnitude();
    auto y_mag = y_mm.magnitude();

    auto x_mm_over_x_mag = x_mm / x_mag;
    auto y_mm_over_y_mag = y_mm / y_mag;

    auto r = x_mm_over_x_mag.dot(y_mm_over_y_mag);

    return std::max(std::min(r, 1.0), -1.0);
}

};
