#!/bin/bash
# Author: David Holmqvist <daae19@student.bth.se>

echo "NOTE: this script runs Valgrind with Memcheck, Callgrind, and Cachegrind for pearson"

status=0
output_dir="valgrind_output"
mkdir -p "$output_dir"  # Ensure the output directory exists

for size in 128 256 512 1024
do
    # Run Memcheck
    echo "Running Valgrind Memcheck on pearson with data/$size.data"
    valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --log-file="$output_dir/${size}_memcheck.txt" ./pearson "data/$size.data" "$output_dir/${size}_seq.data"
    echo "Done running Valgrind Memcheck on pearson with data/$size.data"
    
    # Remove the temporary output file to avoid clutter
    rm "$output_dir/${size}_seq.data"
    
    # Run Callgrind
    echo "Running Valgrind Callgrind on pearson with data/$size.data"
    valgrind --tool=callgrind --callgrind-out-file="$output_dir/${size}_callgrind.out" ./pearson "data/$size.data" "$output_dir/${size}_seq.data"
    callgrind_annotate "$output_dir/${size}_callgrind.out" > "$output_dir/${size}_callgrind_output.txt"
    echo "Done running Valgrind Callgrind on pearson with data/$size.data"
    
    # Remove the temporary output file to avoid clutter
    rm "$output_dir/${size}_seq.data"

    # Run Cachegrind
    echo "Running Valgrind Cachegrind on pearson with data/$size.data"
    valgrind --tool=cachegrind --cachegrind-out-file="$output_dir/${size}_cachegrind.out" ./pearson "data/$size.data" "$output_dir/${size}_seq.data"
    echo "Done running Valgrind Cachegrind on pearson with data/$size.data"
    
    # Remove the temporary output file to avoid clutter
    rm "$output_dir/${size}_seq.data"
done

exit $status
