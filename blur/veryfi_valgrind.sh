#!/bin/bash
# Author: David Holmqvist <daae19@student.bth.se>

echo "NOTE: this script runs Valgrind with Callgrind for the blur program"

status=0
output_dir="valgrind_output"
mkdir -p "$output_dir"  # Ensure the output directory exists

# Array of image names to test
for thread in 1 2 4 8 16 32
do
    for image in im1 im2 im3 im4
    do
        # Run Valgrind Callgrind for the blur program
        echo "Running Valgrind Callgrind on blur with data/$image.ppm and $thread threads"
        
        # Callgrind, save results in valgrind_output directory
        valgrind --tool=callgrind --callgrind-out-file="$output_dir/callgrind_${image}_${thread}.out" \
            ./blur_par 15 "data/$image.ppm" "$output_dir/blur_${image}_${thread}_seq.ppm" $thread
        
        # Annotate the Callgrind output
        callgrind_annotate "$output_dir/callgrind_${image}_${thread}.out" > "$output_dir/${image}_${thread}_callgrind_output.txt"
        
        # Clean up the Callgrind output file after annotation
        rm "$output_dir/callgrind_${image}_${thread}.out"
        
        echo "Done running Valgrind Callgrind on blur with data/$image.ppm and $thread threads"
    done
done

exit $status
