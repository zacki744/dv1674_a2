#include <pthread.h>
#include <vector>
#include <cmath>
#include "filters.hpp"
#include "matrix.hpp"
#include "ppm.hpp"
#include <iostream>  // For debugging purposes
#include <unistd.h> // For sleep function

namespace Filter {

// Structure to hold data for each thread
struct ThreadData {
    Matrix* dst;             // Destination matrix
    Matrix* scratch;         // Scratch matrix for intermediate results
    std::vector<double>* weights; // Gaussian weights
    int start_row;           // Starting row for the thread
    int end_row;             // Ending row for the thread
    int radius;              // Radius for Gaussian blur
};

namespace Gauss {
    void get_weights(int n, double *weights_out) {
        for (int i = 0; i <= n; i++) {
            double x = static_cast<double>(i) * max_x / n;
            weights_out[i] = exp(-x * x * pi);
        }
    }
}

// Clamp function to ensure values stay within bounds
inline int clamp(int value, int min, int max) {
    return std::max(min, std::min(value, max));
}

// Function executed by each thread to process a chunk of the image
void* blur_chunk(void* arg) {
    ThreadData* data = static_cast<ThreadData*>(arg);
    Matrix& dst = *(data->dst);
    Matrix& scratch = *(data->scratch);
    int radius = data->radius;
    sleep(1); // For debugging purposes
    printf("Thread: %d, Start row: %d, End row: %d\n", pthread_self(), data->start_row, data->end_row);
    // Horizontal blur pass
    for (int y = data->start_row; y < data->end_row; ++y) {
        for (int x = 0; x < dst.get_x_size(); ++x) {
            double w[Gauss::max_radius]{};
            Gauss::get_weights(radius, w);

            double r = w[0] * dst.r(x, y);
            double g = w[0] * dst.g(x, y);
            double b = w[0] * dst.b(x, y);
            double n = w[0];

            // Apply Gaussian weights
            for (int wi = 1; wi <= radius; wi++) {
                double wc = w[wi];
                
                // Left neighbor
                int x2 = x - wi;
                if (x2 >= 0) {
                    r += wc * dst.r(x2, y);
                    g += wc * dst.g(x2, y);
                    b += wc * dst.b(x2, y);
                    n += wc;
                }
                
                // Right neighbor
                x2 = x + wi;
                if (x2 < dst.get_x_size()) {
                    r += wc * dst.r(x2, y);
                    g += wc * dst.g(x2, y);
                    b += wc * dst.b(x2, y);
                    n += wc;
                }
            }
            // Store the blurred color values in the scratch matrix
            scratch.r(x, y) = r / n;
            scratch.g(x, y) = g / n;
            scratch.b(x, y) = b / n;
        }
    }

    // Vertical blur pass
    for (int x = 0; x < dst.get_x_size(); ++x) {
        for (int y = data->start_row; y < data->end_row; ++y) {
            double w[Gauss::max_radius]{};
            Gauss::get_weights(radius, w);

            double r = w[0] * scratch.r(x, y);
            double g = w[0] * scratch.g(x, y);
            double b = w[0] * scratch.b(x, y);
            double n = w[0];

            // Apply Gaussian weights
            for (int wi = 1; wi <= radius; wi++) {
                double wc = w[wi];
                
                // Upper neighbor
                int y2 = y - wi;
                if (y2 >= 0) {
                    r += wc * scratch.r(x, y2);
                    g += wc * scratch.g(x, y2);
                    b += wc * scratch.b(x, y2);
                    n += wc;
                }
                
                // Lower neighbor
                y2 = y + wi;
                if (y2 < dst.get_y_size()) {
                    r += wc * scratch.r(x, y2);
                    g += wc * scratch.g(x, y2);
                    b += wc * scratch.b(x, y2);
                    n += wc;
                }
            }
            // Store the final blurred color values in the destination matrix
            dst.r(x, y) = r / n;
            dst.g(x, y) = g / n;
            dst.b(x, y) = b / n;
        }
    }

    return nullptr; // Thread has finished execution
}

// Main blur function that creates threads and manages blurring
Matrix blur(Matrix m, const int radius, int num_threads) {
    Matrix scratch{PPM::max_dimension}; // Intermediate results
    Matrix dst = m; // Copy of the original matrix

    pthread_t threads[num_threads]; // Thread handles
    ThreadData thread_data[num_threads]; // Thread data for each thread

    // Get total rows to process
    int total_rows = dst.get_y_size();
    int rows_per_thread = total_rows / num_threads;
    int extra_rows = total_rows % num_threads;

    // Initialize and create threads
    for (int i = 0; i < num_threads; ++i) {
        thread_data[i].start_row = i * rows_per_thread;
        thread_data[i].end_row = (i == num_threads - 1) ? (total_rows) : (thread_data[i].start_row + rows_per_thread);
        
        // Ensure row indices are within bounds
        thread_data[i].start_row = clamp(thread_data[i].start_row, 0, total_rows);
        thread_data[i].end_row = clamp(thread_data[i].end_row, 0, total_rows);

        // Set data for the thread
        thread_data[i].dst = &dst;
        thread_data[i].scratch = &scratch;
        thread_data[i].radius = radius;

        // Create thread
        sleep(1); // For debugging purposes
        pthread_create(&threads[i], nullptr, blur_chunk, &thread_data[i]);
    }

    // Wait for all threads to finish
    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], nullptr);
    }

    return dst; // Return the blurred image
}

} // namespace Filter
