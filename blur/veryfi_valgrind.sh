#!/bin/bash
# Author: David Holmqvist <daae19@student.bth.se>

echo "NOTE: this script runs Valgrind with Memcheck, Callgrind, and Cachegrind for blur and threshold (single-threaded only)"

status=0
output_dir="valgrind_output"
mkdir -p "$output_dir"  # Ensure the output directory exists

# Array of image names to test
images=("im1" "im2" "im3" "im4")

for image in "${images[@]}"
do
    # Run Callgrind for blur
    echo "Running Valgrind Callgrind on blur with data/$image.ppm"
    valgrind --tool=callgrind --callgrind-out-file="$output_dir/callgrind_${image}.out" ./blur 15 "data/$image.ppm" "$output_dir/blur_${image}_seq.ppm"
    callgrind_annotate "$output_dir/callgrind_${image}.out" > "$output_dir/${image}_blur_callgrind_output.txt"
    echo "Done running Valgrind Callgrind on blur with data/$image.ppm"
    
    # Run Cachegrind for blur
    echo "Running Valgrind Cachegrind on blur with data/$image.ppm"
    valgrind --tool=cachegrind --cachegrind-out-file="$output_dir/cachegrind_${image}.out" ./blur 15 "data/$image.ppm" "$output_dir/blur_${image}_seq.ppm"
    
    # Condensed Cachegrind output using cg_annotate
    echo "Generating condensed Cachegrind output for $image.ppm"
    cg_annotate "$output_dir/cachegrind_${image}.out" > "$output_dir/${image}_blur_cachegrind_output.txt"
    echo "Done running Cachegrind on blur with data/$image.ppm"

    # Remove the temporary output file to avoid clutter
    rm "$output_dir/blur_${image}_seq.ppm"
done

exit $status
