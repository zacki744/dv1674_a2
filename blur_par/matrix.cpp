/*
Author: David Holmqvist <daae19@student.bth.se>
*/

#include "matrix.hpp"
#include "ppm.hpp"
#include <fstream>
#include <stdexcept>

Matrix::Matrix(unsigned char* R, unsigned char* G, unsigned char* B, unsigned x_size, unsigned y_size, unsigned color_max)
    : R { R }
    , G { G }
    , B { B }
    , x_size { x_size }
    , y_size { y_size }
    , color_max { color_max }
{
}

Matrix::Matrix()
    : Matrix {
        nullptr,
        nullptr,
        nullptr,
        0,
        0,
        0,
    }
{
}

Matrix::Matrix(unsigned dimension)
    : R { new unsigned char[dimension * dimension] }
    , G { new unsigned char[dimension * dimension] }
    , B { new unsigned char[dimension * dimension] }
    , x_size { dimension }
    , y_size { dimension }
    , color_max { 0 }
{
}

Matrix::Matrix(Matrix& other)
    : R { new unsigned char[other.x_size * other.y_size] }
    , G { new unsigned char[other.x_size * other.y_size] }
    , B { new unsigned char[other.x_size * other.y_size] }
    , x_size { other.x_size }
    , y_size { other.y_size }
    , color_max { other.color_max }
{
    for (auto x { 0 }; x < x_size; x++) {
        for (auto y { 0 }; y < y_size; y++) {
            auto index {x + y * x_size};
            auto &r_val { r(index) }, &g_val { g(index) }, &b_val { b(index) };
            auto other_r_val { other.r(index) }, other_g_val { other.g(index) }, other_b_val { other.b(index) };

            r_val = other_r_val;
            g_val = other_g_val;
            b_val = other_b_val;
        }
    }
}

Matrix& Matrix::operator=(Matrix other)
{
    if (this == &other) {
        return *this;
    }

    this->~Matrix();

    R = new unsigned char[other.x_size * other.y_size];
    G = new unsigned char[other.x_size * other.y_size];
    B = new unsigned char[other.x_size * other.y_size];

    x_size = other.x_size;
    y_size = other.y_size;
    color_max = other.color_max;

    for (auto x { 0 }; x < x_size; x++) {
        for (auto y { 0 }; y < y_size; y++) {
            auto index {x + y * x_size};
            auto &r_val { r(index) }, &g_val { g(index) }, &b_val { b(index) };
            auto other_r_val { other.r(index) }, other_g_val { other.g(index) }, other_b_val { other.b(index) };

            r_val = other_r_val;
            g_val = other_g_val;
            b_val = other_b_val;
        }
    }

    return *this;
}

Matrix::~Matrix()
{
    if (R) {
        delete[] R;
        R = nullptr;
    }
    if (G) {
        delete[] G;
        G = nullptr;
    }
    if (B) {
        delete[] B;
        B = nullptr;
    }

    x_size = y_size = color_max = 0;
}

unsigned Matrix::get_x_size() const
{
    return x_size;
}

unsigned Matrix::get_y_size() const
{
    return y_size;
}

unsigned Matrix::get_color_max() const
{
    return color_max;
}

//5 Deleted const in order to allow the R, G, and B to be accessed directly for performing blurring operation
unsigned char* Matrix::get_R()
{
    return R;
}

unsigned char* Matrix::get_G()
{
    return G;
}

unsigned char* Matrix::get_B()
{
    return B;
}

//4 Index is being pre-calculated in the loop and then used to access values for colors
unsigned char Matrix::r(unsigned index) const
{
    return R[index];
}

unsigned char Matrix::g(unsigned index) const
{
    return G[index];
}

unsigned char Matrix::b(unsigned index) const
{
    return B[index];
}

unsigned char& Matrix::r(unsigned index)
{
    return R[index];
}

unsigned char& Matrix::g(unsigned index)
{
    return G[index];
}

unsigned char& Matrix::b(unsigned index)
{
    return B[index];
}