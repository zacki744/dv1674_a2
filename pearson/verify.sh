#!/bin/bash

echo "NOTE: this script relies on the binaries pearson and pearson_par to exist"

status=0
red=$(tput setaf 1)
yellow=$(tput setaf 3)
reset=$(tput sgr0)

# Function to compare floats with precision of 0.01 (2 decimals)
# Function to compare floats with precision up to 16 decimal places
compare_floats() {
    awk -v a="$1" -v b="$2" 'BEGIN {
        split(a, a_parts, ".")
        split(b, b_parts, ".")
        if (a_parts[1] != b_parts[1]) exit 1;  # Check integer part
        
        # Compare fractional part up to the first 17 decimals
        a_frac = substr(a_parts[2], 1, 17)
        b_frac = substr(b_parts[2], 1, 17)

        if (a_frac == b_frac) {
            exit 0;  # No difference
        }

        # Now, compare the first 12 decimals and then the last two separately
        a_major = substr(a_frac, 1, 15)
        b_major = substr(b_frac, 1, 15)

        a_minor = substr(a_frac, 16, 2)
        b_minor = substr(b_frac, 16, 2)

        if (a_major == b_major && a_minor != b_minor) {
            exit 2;  # Minor difference (last two digits)
        }

        exit 1;  # Major difference
    }'
}

for thread in 2 4 8 16 32
do
    for size in 128 256 512 1024
    do
        ./pearson_par "data/$size.data" "./data_o/${size}_par.data" $thread

        # Compare the files line by line
        difference_found=0
        warning_issued=0
        while IFS= read -r seq_line && IFS= read -r par_line <&3
        do
            compare_floats "$seq_line" "$par_line"
            result=$?
            
            if [ "$result" -eq 1 ]
            then
                # Report major differences as errors
                echo "${red}Error: Incongruent output data detected with size $size and $thread thread(s)${reset}"
                status=1
                difference_found=1
                break

            elif [ "$result" -eq 2 ]
            then
                # Issue warnings for minor differences (last two decimals)
                if [ "$warning_issued" -eq 0 ]; then
                    echo "${yellow}Warning: Minor differences (last two digits) detected for size $size and $thread thread(s)${reset}"
                    warning_issued=1
                fi
            fi
        done < "./data_o/${size}_seq.data" 3< "./data_o/${size}_par.data"

        echo "./data_o/${size}_seq.data"  "./data_o/${size}_par.data"

        if [ "$difference_found" -eq 0 ]; then
            echo "Files match for size $size with $thread thread(s)"
        fi

        rm "./data_o/${size}_par.data"
    done
done

exit $status
