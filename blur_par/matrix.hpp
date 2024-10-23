
/*
Author: David Holmqvist <daae19@student.bth.se>
*/

#include <iostream>

#if !defined(MATRIX_HPP)
#define MATRIX_HPP

class Matrix {
private:
    unsigned char* R;
    unsigned char* G;
    unsigned char* B;

    unsigned x_size;
    unsigned y_size;
    unsigned color_max;

public:
    Matrix();
    Matrix(unsigned dimension);
    Matrix(Matrix& other);
    Matrix(unsigned char* R, unsigned char* G, unsigned char* B, unsigned x_size, unsigned y_size, unsigned color_max);
    Matrix& operator=(Matrix other);
    ~Matrix();

    unsigned get_x_size() const;
    unsigned get_y_size() const;
    unsigned get_color_max() const;

    unsigned char* get_R();
    unsigned char* get_G();
    unsigned char* get_B();


    unsigned char r(unsigned index) const;
    unsigned char g(unsigned index) const;
    unsigned char b(unsigned index) const;
    unsigned char& r(unsigned index);
    unsigned char& g(unsigned index);
    unsigned char& b(unsigned index);
};

#endif