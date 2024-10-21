#include "filters.hpp"
#include "matrix.hpp"
#include "ppm.hpp"
#include <cmath>
#include <vector>

namespace Filter
{
    namespace Gauss
    {
        std::vector<double> get_weights(int n)
        {
            std::vector<double> weights(n + 1);
            for (int i = 0; i <= n; ++i)
            {
                double x = static_cast<double>(i) * max_x / n;
                weights[i] = exp(-x * x * pi);
            }
            return weights;
        }
    }

    Matrix blur(Matrix m, const int radius) {
    Matrix buffer = m;
    auto* src = &m;
    auto* dst = &buffer;

    std::vector<double> weights = Gauss::get_weights(radius);

    for (int y = 0; y < src->get_y_size(); ++y) {
        for (int x = 0; x < src->get_x_size(); ++x) {
            double r = weights[0] * src->r(x, y);
            double g = weights[0] * src->g(x, y);
            double b = weights[0] * src->b(x, y);
            double n = weights[0];

            for (int wi = 1; wi <= radius; ++wi) {
                double wc = weights[wi];
                int x_left = x - wi, x_right = x + wi;

                if (x_left >= 0) {
                    r += wc * src->r(x_left, y);
                    g += wc * src->g(x_left, y);
                    b += wc * src->b(x_left, y);
                    n += wc;
                }
                if (x_right < src->get_x_size()) {
                    r += wc * src->r(x_right, y);
                    g += wc * src->g(x_right, y);
                    b += wc * src->b(x_right, y);
                    n += wc;
                }
            }
            dst->r(x, y) = r / n;
            dst->g(x, y) = g / n;
            dst->b(x, y) = b / n;
        }
    }

    std::swap(src, dst);

    for (int x = 0; x < src->get_x_size(); ++x) {
        for (int y = 0; y < src->get_y_size(); ++y) {
            double r = weights[0] * src->r(x, y);
            double g = weights[0] * src->g(x, y);
            double b = weights[0] * src->b(x, y);
            double n = weights[0];

            for (int wi = 1; wi <= radius; ++wi) {
                double wc = weights[wi];
                int y_up = y - wi, y_down = y + wi;

                if (y_up >= 0) {
                    r += wc * src->r(x, y_up);
                    g += wc * src->g(x, y_up);
                    b += wc * src->b(x, y_up);
                    n += wc;
                }
                if (y_down < src->get_y_size()) {
                    r += wc * src->r(x, y_down);
                    g += wc * src->g(x, y_down);
                    b += wc * src->b(x, y_down);
                    n += wc;
                }
            }
            dst->r(x, y) = r / n;
            dst->g(x, y) = g / n;
            dst->b(x, y) = b / n;
        }
    }

    return *dst;
}
}
