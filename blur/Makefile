# Author: David Holmqvist <daae19@student.bth.se>

CXX=g++
CXXFLAGS=-std=c++17 -g -Wunused -Wall -fpermissive -fopenmp

all: blur blur_par

# Rule for threshold
threshold: matrix ppm filters threshold.cpp
	$(CXX) $(CXXFLAGS) threshold.cpp matrix.o ppm.o filters.o -o threshold

# Rule for blur
blur: matrix ppm filters blur.cpp
	$(CXX) $(CXXFLAGS) blur.cpp matrix.o ppm.o filters.o -o blur

# Rule for parallel blur
blur_par: matrix_par ppm_par filters_par blur_par.cpp
	$(CXX) $(CXXFLAGS) blur_par.cpp matrix_par.o ppm_par.o filters_par.o -o blur_par -pthread

# Rule for filters (sequential)
filters: matrix filters.hpp filters.cpp
	$(CXX) $(CXXFLAGS) -c filters.cpp -o filters.o

# Rule for filters (parallel)
filters_par: matrix_par filters_par.hpp filters_par.cpp
	$(CXX) $(CXXFLAGS) -c filters_par.cpp -o filters_par.o

# Rule for matrix (sequential)
matrix: matrix.hpp matrix.cpp
	$(CXX) $(CXXFLAGS) -c matrix.cpp -o matrix.o

# Rule for matrix (parallel)
matrix_par: matrix_par.hpp matrix_par.cpp
	$(CXX) $(CXXFLAGS) -c matrix_par.cpp -o matrix_par.o

# Rule for ppm (sequential)
ppm: ppm.hpp ppm.cpp
	$(CXX) $(CXXFLAGS) -c ppm.cpp -o ppm.o

# Rule for ppm (parallel)
ppm_par: ppm_par.hpp ppm_par.cpp
	$(CXX) $(CXXFLAGS) -c ppm_par.cpp -o ppm_par.o

clean:
	rm -rf blur threshold blur_par *.ppm *.o *.dSYM 2> /dev/null
