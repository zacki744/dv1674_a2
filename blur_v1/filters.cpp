#include "filters.hpp"
#include "matrix.hpp"
#include "ppm.hpp"
#include <cmath>
#include <vector>

namespace Filter
{
    namespace Gauss
    {
        // Precompute Gaussian weights once for a given radius.
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

    Matrix blur(Matrix m, const int radius)
    {
        Matrix scratch{PPM::max_dimension};
        auto dst{m};

        // Precompute Gaussian weights once.
        std::vector<double> weights = Gauss::get_weights(radius);

        // Horizontal blur pass
        for (int y = 0; y < dst.get_y_size(); ++y)
        {
            for (int x = 0; x < dst.get_x_size(); ++x)
            {
                double r = weights[0] * dst.r(x, y);
                double g = weights[0] * dst.g(x, y);
                double b = weights[0] * dst.b(x, y);
                double n = weights[0];

                // Process neighboring pixels horizontally.
                for (int wi = 1; wi <= radius; ++wi)
                {
                    double wc = weights[wi];
                    int x_left = x - wi;
                    if (x_left >= 0)
                    {
                        r += wc * dst.r(x_left, y);
                        g += wc * dst.g(x_left, y);
                        b += wc * dst.b(x_left, y);
                        n += wc;
                    }
                    int x_right = x + wi;
                    if (x_right < dst.get_x_size())
                    {
                        r += wc * dst.r(x_right, y);
                        g += wc * dst.g(x_right, y);
                        b += wc * dst.b(x_right, y);
                        n += wc;
                    }
                }
                scratch.r(x, y) = r / n;
                scratch.g(x, y) = g / n;
                scratch.b(x, y) = b / n;
            }
        }

        // Vertical blur pass
        for (int x = 0; x < dst.get_x_size(); ++x)
        {
            for (int y = 0; y < dst.get_y_size(); ++y)
            {
                double r = weights[0] * scratch.r(x, y);
                double g = weights[0] * scratch.g(x, y);
                double b = weights[0] * scratch.b(x, y);
                double n = weights[0];

                // Process neighboring pixels vertically.
                for (int wi = 1; wi <= radius; ++wi)
                {
                    double wc = weights[wi];
                    int y_up = y - wi;
                    if (y_up >= 0)
                    {
                        r += wc * scratch.r(x, y_up);
                        g += wc * scratch.g(x, y_up);
                        b += wc * scratch.b(x, y_up);
                        n += wc;
                    }
                    int y_down = y + wi;
                    if (y_down < dst.get_y_size())
                    {
                        r += wc * scratch.r(x, y_down);
                        g += wc * scratch.g(x, y_down);
                        b += wc * scratch.b(x, y_down);
                        n += wc;
                    }
                }
                dst.r(x, y) = r / n;
                dst.g(x, y) = g / n;
                dst.b(x, y) = b / n;
            }
        }

        return dst;
    }
}
