#include <pthread.h>
#include <vector>
#include <cmath>
#include "filters.hpp"
#include "matrix.hpp"
#include "ppm.hpp"
#include <iostream>  // For debugging purposes

namespace Filter {

    struct ThreadData {
        Matrix* dst;
        Matrix* scratch;
        std::vector<double>* weights;
        int start_row;
        int end_row;
        int radius;
    };

    namespace Gauss {
        void get_weights(int n, std::vector<double>& weights) {
            for (int i = 0; i <= n; ++i) {
                double x = static_cast<double>(i) * max_x / n;
                weights[i] = exp(-x * x * pi);
            }
        }
    }

    inline int clamp(int value, int min, int max) {
        return std::max(min, std::min(value, max));
    }

    // Vertical blur pass (top to bottom)
    void* blur_vertical_chunk(void* arg) {
        ThreadData* data = static_cast<ThreadData*>(arg);
        Matrix& dst = *(data->dst);
        Matrix& scratch = *(data->scratch);
        std::vector<double>& weights = *(data->weights);
        int radius = data->radius;

        for (int x = 0; x < dst.get_x_size(); ++x) {
            for (int y = data->start_row; y < data->end_row; ++y) {
                double r = weights[0] * dst.r(x, y);
                double g = weights[0] * dst.g(x, y);
                double b = weights[0] * dst.b(x, y);
                double n = weights[0];

                for (int wi = 1; wi <= radius; ++wi) {
                    double wc = weights[wi];
                    int y_up = clamp(y - wi, 0, dst.get_y_size() - 1);
                    int y_down = clamp(y + wi, 0, dst.get_y_size() - 1);

                    r += wc * (dst.r(x, y_up) + dst.r(x, y_down));
                    g += wc * (dst.g(x, y_up) + dst.g(x, y_down));
                    b += wc * (dst.b(x, y_up) + dst.b(x, y_down));
                    n += 2 * wc;
                }
                scratch.r(x, y) = r / n;
                scratch.g(x, y) = g / n;
                scratch.b(x, y) = b / n;
            }
        }

        return nullptr;
    }

    // Horizontal blur pass (left to right)
    void* blur_horizontal_chunk(void* arg) {
        ThreadData* data = static_cast<ThreadData*>(arg);
        Matrix& dst = *(data->dst);
        Matrix& scratch = *(data->scratch);
        std::vector<double>& weights = *(data->weights);
        int radius = data->radius;

        for (int y = data->start_row; y < data->end_row; ++y) {
            for (int x = 0; x < dst.get_x_size(); ++x) {
                double r = weights[0] * scratch.r(x, y);
                double g = weights[0] * scratch.g(x, y);
                double b = weights[0] * scratch.b(x, y);
                double n = weights[0];

                for (int wi = 1; wi <= radius; ++wi) {
                    double wc = weights[wi];
                    int x_left = clamp(x - wi, 0, dst.get_x_size() - 1);
                    int x_right = clamp(x + wi, 0, dst.get_x_size() - 1);

                    r += wc * (scratch.r(x_left, y) + scratch.r(x_right, y));
                    g += wc * (scratch.g(x_left, y) + scratch.g(x_right, y));
                    b += wc * (scratch.b(x_left, y) + scratch.b(x_right, y));
                    n += 2 * wc;
                }
                dst.r(x, y) = r / n;
                dst.g(x, y) = g / n;
                dst.b(x, y) = b / n;
            }
        }

        return nullptr;
    }

    Matrix blur(Matrix m, const int radius, int num_threads) {
        Matrix scratch{PPM::max_dimension};
        Matrix dst = m;

        // Precompute weights
        std::vector<double> weights(radius + 1);
        Gauss::get_weights(radius, weights);

        pthread_t threads[num_threads];
        ThreadData thread_data[num_threads];

        // Get total rows to process
        int total_rows = dst.get_y_size();
        int rows_per_thread = total_rows / num_threads;

        // Vertical pass (parallel)
        for (int i = 0; i < num_threads; ++i) {
            thread_data[i].start_row = i * rows_per_thread;
            thread_data[i].end_row = (i == num_threads - 1) ? total_rows : (thread_data[i].start_row + rows_per_thread);

            thread_data[i].start_row = clamp(thread_data[i].start_row, 0, total_rows);
            thread_data[i].end_row = clamp(thread_data[i].end_row, 0, total_rows);

            thread_data[i].dst = &dst;
            thread_data[i].scratch = &scratch;
            thread_data[i].weights = &weights;
            thread_data[i].radius = radius;

            pthread_create(&threads[i], nullptr, blur_vertical_chunk, &thread_data[i]);
        }

        // Wait for vertical blur threads to finish
        for (int i = 0; i < num_threads; ++i) {
            pthread_join(threads[i], nullptr);
        }

        // Horizontal pass (parallel)
        for (int i = 0; i < num_threads; ++i) {
            pthread_create(&threads[i], nullptr, blur_horizontal_chunk, &thread_data[i]);
        }

        // Wait for horizontal blur threads to finish
        for (int i = 0; i < num_threads; ++i) {
            pthread_join(threads[i], nullptr);
        }

        return dst;
    }

}  // namespace Filter
