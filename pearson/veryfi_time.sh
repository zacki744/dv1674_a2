#!/bin/bash
# Author: David Holmqvist <daae19@student.bth.se>

echo "NOTE: this script captures output and measures execution time for the pearson program (pearson_par is not yet implemented)"

status=0
output_dir="time_output"
mkdir -p "$output_dir"  # Ensure the output directory exists

for size in 128 256 512 1024
do
    echo "Running pearson with data/$size.data"

    # Measure execution time and capture output
    {
        echo "Measuring execution time for pearson with data/$size.data"
        /usr/bin/time -v ./pearson "data/$size.data" "$output_dir/${size}_seq.data"  # Using -v for detailed timing
    } &> "$output_dir/${size}_output.txt"  # Capture both stdout and stderr

    echo "Output and time measurements for $size.data saved to $output_dir/${size}_output.txt"
done

exit $status
