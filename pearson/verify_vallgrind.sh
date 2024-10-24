#!/bin/bash
# Author: David Holmqvist <daae19@student.bth.se>

echo "NOTE: this script runs Valgrind with Memcheck, Callgrind, and Cachegrind for pearson"

status=0
output_dir="valgrind_output"
mkdir -p "$output_dir"  # Ensure the output directory exists

for thread in 1 2 4 8 16 32
do
    for size in 128 256 512 1024
    do
        # Run Valgrind Callgrind
        echo "Running Valgrind Callgrind on pearson with data/$size.data and $thread threads"
        valgrind --tool=callgrind --callgrind-out-file="$output_dir/${size}_${thread}_callgrind.out" ./pearson_par "data/$size.data" "$output_dir/${size}_${thread}_par.data" $thread
        echo "Done running Valgrind Callgrind on pearson with data/$size.data and $thread threads"
        # Condensed Cachegrind output using cg_annotate
        echo "Generating condensed Callgrind output for $size.data with $thread threads"
        callgrind_annotate "$output_dir/${size}_${thread}_callgrind.out" > "$output_dir/${size}_${thread}_callgrind_output.txt"
        echo "Done generating condensed Callgrind output for $size.data with $thread threads"

        # Remove the temporary output and res file to avoid clutter
        rm "$output_dir/${size}_${thread}_par.data" "$output_dir/${size}_${thread}_callgrind.out"
    done
done

exit $status
