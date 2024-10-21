/*
Author: David Holmqvist <daae19@student.bth.se>
*/

#include "vector.hpp"
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <stdexcept>

// Default Constructor
Vector::Vector()
    : size{0}, data{nullptr}
{
}

// Destructor
Vector::~Vector()
{
    delete[] data; // No need to check if `data` is nullptr (safe with `delete[]`)
}

// Constructor with size
Vector::Vector(unsigned size)
    : size{size}, data{new double[size]}
{
}

// Constructor with external data pointer
Vector::Vector(unsigned size, double* data)
    : size{size}, data{new double[size]}
{
    std::copy(data, data + size, this->data);
}

// Copy Constructor (Optimized)
Vector::Vector(const Vector& other)
    : size{other.size}, data{new double[other.size]}
{
    std::copy(other.data, other.data + size, data);
}

// Get size of the vector
unsigned Vector::get_size() const
{
    return size;
}

// Get raw data pointer
double* Vector::get_data()
{
    return data;
}

// Const subscript operator (inlined)
inline double Vector::operator[](unsigned i) const
{
    return data[i];
}

// Non-const subscript operator (inlined)
inline double& Vector::operator[](unsigned i)
{
    return data[i];
}

// Compute the mean of the vector (Optimized with loop unrolling)
double Vector::mean() const
{
    if (size == 0) return 0.0; // Avoid division by zero

    double sum = 0;
    const double* data_ptr = data; // Cache pointer for faster access

    unsigned i = 0;
    // Loop unrolling: process 4 elements per iteration
    for (; i + 3 < size; i += 4)
    {
        sum += data_ptr[i] + data_ptr[i + 1] +
               data_ptr[i + 2] + data_ptr[i + 3];
    }
    // Handle remaining elements
    for (; i < size; ++i)
    {
        sum += data_ptr[i];
    }

    return sum / size;
}

// Compute the magnitude of the vector
double Vector::magnitude() const
{
    return std::sqrt(dot(*this)); // Use cached `dot()` result
}

// Division operator (Optimized: Avoid subscript calls)
Vector Vector::operator/(double div)
{
    if (div == 0.0)
        throw std::runtime_error("Division by zero.");

    Vector result(size);
    const double* src_data = data;
    double* dest_data = result.data;

    for (unsigned i = 0; i < size; ++i)
    {
        dest_data[i] = src_data[i] / div;
    }

    return result;
}

// Subtraction operator (Optimized: Avoid subscript calls)
Vector Vector::operator-(double sub)
{
    Vector result(size);
    const double* src_data = data;
    double* dest_data = result.data;

    for (unsigned i = 0; i < size; ++i)
    {
        dest_data[i] = src_data[i] - sub;
    }

    return result;
}

double Vector::dot(Vector rhs) const
{
    if (size != rhs.size)
        throw std::invalid_argument("Vectors must be of the same size.");

    double result = 0;
    const double* lhs_data = data;
    const double* rhs_data = rhs.data;

    unsigned i = 0;
    // Loop unrolling: process 4 elements per iteration
    for (; i + 3 < size; i += 4)
    {
        result += lhs_data[i] * rhs_data[i] +
                  lhs_data[i + 1] * rhs_data[i + 1] +
                  lhs_data[i + 2] * rhs_data[i + 2] +
                  lhs_data[i + 3] * rhs_data[i + 3];
    }
    // Handle remaining elements
    for (; i < size; ++i)
    {
        result += lhs_data[i] * rhs_data[i];
    }

    return result;
}