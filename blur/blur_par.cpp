/*
Author: David Holmqvist <daae19@student.bth.se>
*/

#include "matrix_par.hpp"
#include "ppm.hpp"
#include "filters_par.hpp"
#include <cstdlib>
#include <iostream>

int main(int argc, char const* argv[])
{
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " [radius] [infile] [outfile] [threads]" << std::endl;
        std::exit(1);
    }

    PPM::Reader reader {};
    PPM::Writer writer {};

    auto m { reader(argv[2]) };
    auto radius { static_cast<unsigned>(std::stoul(argv[1])) };
    auto num_threads { static_cast<int>(std::stoul(argv[4])) };

    auto blurred { Filter::blur(m, radius, num_threads) };
    writer(blurred, argv[3]);

    return 0;
}

