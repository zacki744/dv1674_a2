#!/bin/bash
# Author: David Holmqvist <daae19@student.bth.se>

echo "NOTE: this script runs Valgrind with Memcheck, Callgrind, and Cachegrind for blur and threshold"

status=0
output_dir="valgrind_output"
mkdir -p "$output_dir"  # Ensure the output directory exists

for size in 128 256 512 1024
do
    for image in im1 im2 im3 im4
    do
        # Run Memcheck for blur
        echo "Running Valgrind Memcheck on blur with data/$image.ppm"
        valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --log-file="$output_dir/${image}_blur_memcheck.txt" ./blur 15 "data/$image.ppm" "$output_dir/blur_${image}_seq.ppm"
        echo "Done running Valgrind Memcheck on blur with data/$image.ppm"
        
        # Run Callgrind for blur
        echo "Running Valgrind Callgrind on blur with data/$image.ppm"
        valgrind --tool=callgrind --callgrind-out-file="$output_dir/callgrind.out.$$" ./blur 15 "data/$image.ppm" "$output_dir/blur_${image}_seq.ppm"
        callgrind_annotate "$output_dir/callgrind.out.$$" > "$output_dir/${image}_blur_callgrind_output.txt"
        echo "Done running Valgrind Callgrind on blur with data/$image.ppm"
        
        # Run Cachegrind for blur
        echo "Running Valgrind Cachegrind on blur with data/$image.ppm"
        valgrind --tool=cachegrind --cachegrind-out-file="$output_dir/${image}_blur_cachegrind.out" ./blur 15 "data/$image.ppm" "$output_dir/blur_${image}_seq.ppm"
        echo "Done running Valgrind Cachegrind on blur with data/$image.ppm"
        
        # Remove the temporary output file to avoid clutter
        rm "$output_dir/blur_${image}_seq.ppm"
    done
done

exit $status
