/*
Author: David Holmqvist <daae19@student.bth.se>
*/

#include "filters.hpp"
#include "matrix.hpp"
#include "ppm.hpp"
#include <cmath>
#include <pthread.h>

namespace Filter {

    namespace Gauss {

        void get_weights(int n, double *weights_out) {
            for (int i = 0; i <= n; i++) {
                double x = static_cast<double>(i) * max_x / n;
                weights_out[i] = exp(-x * x * pi);
            }
        }
    }

    // Structure to hold data needed by each thread during the blur operation
    struct BlurTaskData {
        Matrix* target_matrix;      // Pointer to the target matrix (either scratch or final)
        unsigned char* red_channel; // Pointer to the red channel data
        unsigned char* green_channel; // Pointer to the green channel data
        unsigned char* blue_channel; // Pointer to the blue channel data
        double* gaussian_weights;   // Pointer to the precomputed Gaussian weights
        int blur_radius;            // The radius for the Gaussian blur
        unsigned img_width;         // Image width (x_size)
        unsigned img_height;        // Image height (y_size)
        int row_start;              // Start row for the thread to process
        int row_end;                // End row for the thread to process
        bool horizontal_pass;       // Indicates if this is a horizontal pass or vertical pass

        // Default constructor for safe initialization
        BlurTaskData()
            : target_matrix(nullptr), red_channel(nullptr), green_channel(nullptr), blue_channel(nullptr),
            gaussian_weights(nullptr), blur_radius(0), img_width(0), img_height(0),
            row_start(0), row_end(0), horizontal_pass(true) {}

        // Parameterized constructor for initializing the task data
        BlurTaskData(Matrix* matrix, unsigned char* R, unsigned char* G, 
                    unsigned char* B, double* weights, int radius,
                    unsigned width, unsigned height, int start_row, int end_row, bool is_horizontal)
            : target_matrix(matrix), red_channel(R), green_channel(G), blue_channel(B),
            gaussian_weights(weights), blur_radius(radius), 
            img_width(width), img_height(height), 
            row_start(start_row), row_end(end_row), horizontal_pass(is_horizontal) {}
    };

    void blur_pixel(int x, int y, unsigned char* R, unsigned char* G, unsigned char* B, Matrix& matrix,
                    const double* weights, int radius, int x_size, int y_size, bool is_horizontal) {
        const int index = x + y * x_size;
        if (index < 0 || index >= x_size * y_size) return;

        double r = 0.0, g = 0.0, b = 0.0, n = 0.0;  // Declare once outside of loops
        const double w0 = weights[0];

        // Initialize with the center pixel
        r = w0 * R[index];
        g = w0 * G[index];
        b = w0 * B[index];
        n = w0;

        if (is_horizontal) {
            // Horizontal pass
            for (int wi = 1; wi <= radius; ++wi) {
                const double wc = weights[wi];

                // Check left pixel
                int x_left = x - wi;
                if (x_left >= 0) {
                    int left_index = x_left + y * x_size;
                    r += wc * R[left_index];
                    g += wc * G[left_index];
                    b += wc * B[left_index];
                    n += wc;
                }

                // Check right pixel
                int x_right = x + wi;
                if (x_right < x_size) {
                    int right_index = x_right + y * x_size;
                    r += wc * R[right_index];
                    g += wc * G[right_index];
                    b += wc * B[right_index];
                    n += wc;
                }
            }
        } else {
            // Vertical pass
            for (int wi = 1; wi <= radius; ++wi) {
                const double wc = weights[wi];

                // Check upper pixel
                int y_up = y - wi;
                if (y_up >= 0) {
                    int up_index = x + y_up * x_size;
                    r += wc * R[up_index];
                    g += wc * G[up_index];
                    b += wc * B[up_index];
                    n += wc;
                }

                // Check lower pixel
                int y_down = y + wi;
                if (y_down < y_size) {
                    int down_index = x + y_down * x_size;
                    r += wc * R[down_index];
                    g += wc * G[down_index];
                    b += wc * B[down_index];
                    n += wc;
                }
            }
        }

        // Assign final blurred value
        matrix.r(index) = r / n;
        matrix.g(index) = g / n;
        matrix.b(index) = b / n;
    }
    void* blur_pass(void* arg) {
        auto* data = static_cast<BlurTaskData*>(arg);
        unsigned char* R = data->red_channel;
        unsigned char* G = data->green_channel;
        unsigned char* B = data->blue_channel;
        double* weights = data->gaussian_weights;
        Matrix* matrix = data->target_matrix;
        const int radius = data->blur_radius;
        const unsigned x_size = data->img_width;
        const unsigned y_size = data->img_height;
        const int start = data->row_start;
        const int end = data->row_end;
        const bool is_horizontal = data->horizontal_pass;

        if (is_horizontal) {
            for (int x = start; x < end; ++x) {
                for (int y = 0; y < y_size; ++y) {
                    blur_pixel(x, y, R, G, B, *matrix, weights, radius, x_size, y_size, true);
                }
            }
        } else {
            for (int y = start; y < end; ++y) {
                for (int x = 0; x < x_size; ++x) {
                    blur_pixel(x, y, R, G, B, *matrix, weights, radius, x_size, y_size, false);
                }
            }
        }

        return nullptr;
    }

    // The blur function itself
    Matrix blur(Matrix m, const int radius, int num_threads) {
        Matrix scratch(PPM::max_dimension);
        auto dst = m;

        // Get image dimensions
        const unsigned x_size = dst.get_x_size();
        const unsigned y_size = dst.get_y_size();

        // Calculate Gaussian weights beforehand
        double weights[Gauss::max_radius] = {};
        Gauss::get_weights(radius, weights);

        // Directly access R, G, and B channels for optimization
        auto dst_R = dst.get_R();
        auto dst_G = dst.get_G();
        auto dst_B = dst.get_B();
        auto scr_R = scratch.get_R();
        auto scr_G = scratch.get_G();
        auto scr_B = scratch.get_B();

        // Introduce multithreading
        pthread_t threads[num_threads];
        BlurTaskData thread_data[num_threads];
        int rows = x_size / num_threads;

        // Horizontal blurring
        for (int i = 0; i < num_threads; i++) {
            thread_data[i] = {
                &scratch,
                dst_R,
                dst_G,
                dst_B,
                weights,
                radius,
                x_size,
                y_size,
                i * rows,
                static_cast<int>(i == num_threads - 1 ? x_size : (i + 1) * rows), // Ensure all rows are processed
                true,
            };
            pthread_create(&threads[i], nullptr, blur_pass, &thread_data[i]);
        }

        for (int i = 0; i < num_threads; i++) {
            pthread_join(threads[i], nullptr);
        }

        // Vertical blurring
        for (int i = 0; i < num_threads; i++) {
            thread_data[i] = {
                &dst,
                scr_R,
                scr_G,
                scr_B,
                weights,
                radius,
                x_size,
                y_size,
                i * rows,
                static_cast<int>(i == num_threads - 1 ? y_size : (i + 1) * rows), // Ensure all rows are processed
                false
            };
            pthread_create(&threads[i], nullptr, blur_pass, &thread_data[i]);
        }

        for (int i = 0; i < num_threads; i++) {
            pthread_join(threads[i], nullptr);
        }

        return dst;
    }

} // namespace Filter
