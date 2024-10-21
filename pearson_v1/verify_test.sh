#!/bin/bash
# Author: David Holmqvist <daae19@student.bth.se>

echo "NOTE: this script relies on the binary pearson to exist (pearson_par is not yet implemented)"

status=0
output_dir="output_data"
mkdir -p "$output_dir"  # Ensure the output directory exists
red=$(tput setaf 1)
reset=$(tput sgr0)

for size in 128 256 512 1024
do
    echo "Running sequential pearson with data/$size.data"
    
    # Start collecting top and sar data in the background
    top -b -d 1 > "$output_dir/${size}_top.txt" &  # Capture top output every second
    top_pid=$!
    
    sar -u 1 > "$output_dir/${size}_sar.txt" &     # Capture CPU usage every second
    sar_pid=$!
    
    # Run pearson and store the output in the output directory
    ./pearson "data/$size.data" "$output_dir/${size}_seq.data"
    
    # Kill background monitoring processes once pearson is done
    kill $top_pid
    kill $sar_pid
    
    # Generate the profiling data for each size
    gprof ./pearson gmon.out > "$output_dir/${size}_res.gprof"
    
    echo "Done running sequential pearson with data/$size.data"
done

exit $status
