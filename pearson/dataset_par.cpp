/*
Author: David Holmqvist <daae19@student.bth.se>
*/

#include "analysis.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <list>
#include <vector>
#include <pthread.h>

struct ThreadData {
    const std::vector<Vector>* datasets; // dataset vector 
    std::vector<double>* result;         // shared result vector
    int start_idx;                       // data starting index for the current thread
    int end_idx;                         // data ending index for the current thread
    int result_start_idx;                // index where the thread should start writing
};

pthread_mutex_t result_mutex;

namespace Analysis {
double pearson(const Vector& vec1, const Vector& vec2) {
    auto x_mean { vec1.mean() };
    auto y_mean { vec2.mean() };

    auto x_mm { vec1 - x_mean };
    auto y_mm { vec2 - y_mean };

    auto x_mag { x_mm.magnitude() };
    auto y_mag { y_mm.magnitude() };

    auto x_mm_over_x_mag { x_mm / x_mag };
    auto y_mm_over_y_mag { y_mm / y_mag };

    auto r { x_mm_over_x_mag.dot(y_mm_over_y_mag) };

    return std::max(std::min(r, 1.0), -1.0);
}
void* compute_correlation(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    const std::vector<Vector>& datasets = *(data->datasets);
    std::vector<double>& result = *(data->result);
    
    int result_idx = data->result_start_idx;

    // compute correlation for the assigned chunck of data
    for (int sample1 = data->start_idx; sample1 < data->end_idx; ++sample1) {
        for (int sample2 = sample1 + 1; sample2 < datasets.size(); ++sample2) {
            double corr = pearson(datasets[sample1], datasets[sample2]);
            result[result_idx++] = corr;
        }
    }

    pthread_exit(nullptr);
}

std::vector<double> correlation_coefficients(const std::vector<Vector>& datasets, int num_threads) {
        int dataset_size = datasets.size();
        
        // calculate the total numbers of dataset paris
        int total_pairs = (dataset_size * (dataset_size - 1)) / 2;
        std::vector<double> result(total_pairs);  // re-allocate result vector
        
        pthread_mutex_init(&result_mutex, nullptr);

        // vectors to store threads and thread-specific data
        std::vector<pthread_t> threads(num_threads);
        std::vector<ThreadData> thread_data(num_threads);

        // compute chunk size for each thread
        int chunk_size = dataset_size / num_threads;
        int result_start_idx = 0;

        // create and lauch threads
        for (int i = 0; i < num_threads; ++i) {
            int start_idx = i * chunk_size;
            int end_idx = (i == num_threads - 1) ? dataset_size : (i + 1) * chunk_size;

            // compute the number of pairs each thread will compute
            int num_pairs = 0;
            for (int sample1 = start_idx; sample1 < end_idx; ++sample1) {
                num_pairs += (dataset_size - sample1 - 1);
            }

            // prepare thread-specific data
            thread_data[i] = { &datasets, &result, start_idx, end_idx, result_start_idx };
            result_start_idx += num_pairs; // update result start index

            pthread_create(&threads[i], nullptr, compute_correlation, (void*)&thread_data[i]);
        }

        // join the threads
        for (int i = 0; i < num_threads; ++i) {
            pthread_join(threads[i], nullptr);
        }

        pthread_mutex_destroy(&result_mutex);

        return result;
    }
};