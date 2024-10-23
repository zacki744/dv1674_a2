/*
Author: David Holmqvist <daae19@student.bth.se>
*/

#include "vector.hpp"
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <stdexcept>

Vector::Vector()
    : size{0}, data{nullptr}
{
}

Vector::~Vector()
{
    delete[] data;
}

Vector::Vector(unsigned size)
    : size{size}, data{new double[size]}
{
}

Vector::Vector(unsigned size, double* data)
    : size{size}, data{new double[size]}
{
    std::copy(data, data + size, this->data);
}

Vector::Vector(const Vector& other)
    : size{other.size}, data{new double[other.size]}
{
    std::copy(other.data, other.data + size, data);
}

unsigned Vector::get_size() const
{
    return size;
}

double* Vector::get_data()
{
    return data;
}

inline double Vector::operator[](unsigned i) const
{
    return data[i];
}

inline double& Vector::operator[](unsigned i)
{
    return data[i];
}

double Vector::mean() const
{
    if (size == 0) return 0.0;

    double sum = 0;
    const double* data_ptr = data;
    unsigned i = 0;
    
    for (; i + 3 < size; i += 4)
    {
        sum += data_ptr[i] + data_ptr[i + 1] +
               data_ptr[i + 2] + data_ptr[i + 3];
    }

    for (; i < size; ++i)
    {
        sum += data_ptr[i];
    }

    return sum / size;
}

double Vector::magnitude() const
{
    return std::sqrt(dot(*this)); 
}

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
    
    for (; i + 3 < size; i += 4)
    {
        result += lhs_data[i] * rhs_data[i] +
                  lhs_data[i + 1] * rhs_data[i + 1] +
                  lhs_data[i + 2] * rhs_data[i + 2] +
                  lhs_data[i + 3] * rhs_data[i + 3];
    }
    
    for (; i < size; ++i)
    {
        result += lhs_data[i] * rhs_data[i];
    }

    return result;
}