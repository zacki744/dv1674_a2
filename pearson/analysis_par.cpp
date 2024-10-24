#include "analysis_par.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>
#include <pthread.h>

namespace Analysis {

struct ThreadData { // Struct to hold the data for each thread
    const std::vector<Vector>* datasets;
    std::vector<double>* results;
    int start;
    int end;
    int offset;
};

void* calculate_correlations(void* arg) { // Function to calculate the correlations
    ThreadData* data = static_cast<ThreadData*>(arg);
    const std::vector<Vector>& datasets = *(data->datasets);
    std::vector<double>& results = *(data->results);
    // Calculate the correlations
    int result_index = data->offset;
    int total_samples = datasets.size();
    // Loop over the pairs of datasets
    for (int i = data->start; i < data->end; ++i) {
        for (int j = i + 1; j < total_samples; ++j) {
            double corr = pearson(datasets[i], datasets[j]);
            results[result_index++] = corr;
        }
    }
    return nullptr;
}
// Function to calculate the correlation coefficients
std::vector<double> correlation_coefficients(std::vector<Vector> datasets, size_t num_threads) {
   // Initialize the result vector
    std::vector<double> result((datasets.size() * (datasets.size() - 1)) / 2);
    // Create the threads
    pthread_t threads[num_threads];
    ThreadData thread_data[num_threads];
    // Calculate the number of pairs per thread
    int datasets_size = datasets.size();
    int total_pairs = (datasets_size * (datasets_size - 1)) / 2;
    int pairs_per_thread = total_pairs / num_threads;
    int leftover_pairs = total_pairs % num_threads;

    int start_index = 0;

    for (int i = 0; i < num_threads; ++i) { // Start the threads
        thread_data[i].datasets = &datasets;
        thread_data[i].results = &result;
        int num_pairs = pairs_per_thread + (i < leftover_pairs ? 1 : 0);
        thread_data[i].start = start_index;
        thread_data[i].end = start_index + num_pairs;
        thread_data[i].offset = start_index;
        start_index += num_pairs;

        pthread_create(&threads[i], nullptr, calculate_correlations, &thread_data[i]); // Create the thread
    }

    for (int i = 0; i < num_threads; ++i) { // Wait for the threads to finish
        pthread_join(threads[i], nullptr);
    }
    return result;
}

double pearson(Vector vec1, Vector vec2) {
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
