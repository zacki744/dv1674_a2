#!/bin/bash
# Author: David Holmqvist <daae19@student.bth.se>

echo "NOTE: this script measures execution time and captures output for the blur program (single-threaded version)"

status=0
output_dir="time_output"
mkdir -p "$output_dir"  # Ensure the output directory exists

for thread in 1 2 4 8 16 32
do
    for image in im1 im2 im3 im4
    do
        echo "Running blur with data/$image.data and $thread threads"

        # Measure execution time and capture output
        {
            echo "Measuring execution time for blur with data/$image.data and $thread threads"
            /usr/bin/time -v ./blur_par 15 "data/$image.ppm" "$output_dir/blur_${image}_par.ppm" $thread # Using -v for detailed timing
        } &> "$output_dir/${image}_${thread}_output.txt"  # Capture both stdout and stderr

        echo "Output and time measurements for $image.data with $thread threads saved to $output_dir/${image}_${thread}_output.txt"
    done
done
exit $status
