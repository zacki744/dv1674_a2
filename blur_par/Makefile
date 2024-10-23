# Author: David Holmqvist <daae19@student.bth.se>

CXX=g++
CXXFLAGS=-std=c++17 -g -Wunused -Wall -pthread -fopenmp -fpermissive

all: blur_par threshold

threshold: matrix ppm filters threshold.cpp
	$(CXX) $(CXXFLAGS) threshold.cpp matrix.o ppm.o filters.o -o threshold

blur_par: matrix ppm filters blur.cpp
	$(CXX) $(CXXFLAGS) blur.cpp matrix.o ppm.o filters.o -o blur_par

filters: matrix filters.hpp filters.cpp
	$(CXX) $(CXXFLAGS) -c filters.cpp -o filters.o

matrix: matrix.hpp matrix.cpp
	$(CXX) $(CXXFLAGS) -c matrix.cpp -o matrix.o

ppm: ppm.hpp ppm.cpp
	$(CXX) $(CXXFLAGS) -c ppm.cpp -o ppm.o

clean:
	rm -rf blur_par threshold *.ppm *.o *.dSYM 2> /dev/null gmon.out
