#!/bin/bash
# Author: David Holmqvist <daae19@student.bth.se>

echo "NOTE: this script measures execution time and captures output for the blur program (single-threaded version)"

status=0
output_dir="time_output_hardwareconcurrency"
mkdir -p "$output_dir"  # Ensure the output directory exists

# Loop through image files
for image in im1 im2 im3 im4
do
    # Measure execution time and capture output
    echo "Measuring execution time for blur with data/$image.ppm"
    
    # Save both the output from the blur program and time measurements in a single file
    {
        /usr/bin/time -v ./blur_par 15 "data/$image.ppm" "$output_dir/blur_${image}_seq.ppm" 12
    } &> "$output_dir/${image}_output.txt"  # Capture both stdout and stderr
    valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --log-file="$output_dir/${image}_memcheck.txt" ./blur_par 15 "data/$image.ppm" "$output_dir/blur_${image}_seq.ppm" 12

    echo "Execution details and output for $image.ppm saved to $output_dir/${image}_output.txt"
done

exit $status
