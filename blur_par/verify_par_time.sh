#!/bin/bash
# Author: David Holmqvist <daae19@student.bth.se>

echo "NOTE: this script measures execution time and captures output for the blur_par program with multiple thread counts"

status=0
output_dir="time_output_threads"
mkdir -p "$output_dir"  # Ensure the output directory exists

# Array of thread counts to test
thread_counts=(1 2 4 8 16 32)

# Loop through image files
for image in im1 im2 im3 im4
do
    for threads in "${thread_counts[@]}"
    do
        # Measure execution time and capture output
        echo "Measuring execution time for blur_par with data/$image.ppm using $threads thread(s)"
        
        # Run the program and time the execution, ensuring the time command applies to the actual execution
        /usr/bin/time -v ./blur 15 "data/$image.ppm" "$output_dir/blur_${image}_${threads}_threads.ppm" $threads &> "$output_dir/${image}_${threads}_output.txt"

        echo "Execution details and output for $image.ppm with $threads thread(s) saved to $output_dir/${image}_${threads}_output.txt"
    done
done

exit $status
