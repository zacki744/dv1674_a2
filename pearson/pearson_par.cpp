/*
Author: David Holmqvist <daae19@student.bth.se>
*/

#include "analysis_par.hpp"
#include "dataset_par.hpp"
#include <iostream>
#include <cstdlib>
#include <string>

int main(int argc, char const* argv[])
{
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " [dataset] [outfile] [thread_count]" << std::endl;
        std::exit(1);
    }

    std::string dataset_file = argv[1];
    std::string output_file = argv[2];
    size_t thread_count = std::stoul(argv[3]);


    auto datasets = Dataset::read(dataset_file);
    if (datasets.empty()) {
        std::cerr << "Error: No valid data found in " << dataset_file << std::endl;
        std::exit(1);
    }

    auto corrs = Analysis::correlation_coefficients(datasets, thread_count);

    Dataset::write(corrs, output_file);

    return 0;
}
