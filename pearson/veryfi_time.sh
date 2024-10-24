#!/bin/bash
# Author: David Holmqvist <daae19@student.bth.se>

echo "NOTE: this script captures output and measures execution time for the pearson program (pearson_par is not yet implemented)"

status=0
output_dir="time_output_1"
mkdir -p "$output_dir"  # Ensure the output directory exists
for thread in 1 2 4 8 16 32
do
    for size in 128 256 512 1024
    do
        echo "Running pearson with data/$size.data and $thread threads"

        # Measure execution time and capture output
        {
            echo "Measuring execution time for pearson with data/$size.data and $thread threads"
            /usr/bin/time -v ./pearson_par "data/$size.data" "$output_dir/${size}_${thread}_par.data" $thread  # Using -v for detailed timing
        } &> "$output_dir/${size}_${thread}_output.txt"  # Capture both stdout and stderr
        
        #remove result file
        

        echo "Output and time measurements for $size.data with $thread threads saved to $output_dir/${size}_${thread}_output.txt"
    done
done

exit $status
