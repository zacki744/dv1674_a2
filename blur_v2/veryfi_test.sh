#!/bin/bash
# Author: David Holmqvist <daae19@student.bth.se>

echo "NOTE: this script runs Valgrind with Memcheck, Callgrind, and Cachegrind for blur (multi-threaded version skipped)"

status=0
output_dir="test_output"
mkdir -p "$output_dir"  # Ensure the output directory exists

# Loop through image files
for image in im1 im2 im3 im4
do
    # Start collecting top and sar data in the background
    echo "Collecting system performance data for $image.ppm"
    top -b -d 1 > "$output_dir/${image}_top.txt" &  # Capture top output every second
    top_pid=$!
    sar -u 1 > "$output_dir/${image}_sar.txt" &     # Capture CPU usage every second
    sar_pid=$!

    ./blur 15 "data/$image.ppm" "$output_dir/blur_${image}_seq.ppm"

    # Kill background monitoring processes once blur is done
    kill $top_pid
    kill $sar_pid
done

exit $status
