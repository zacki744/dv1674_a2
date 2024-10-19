#!/bin/bash

status=0
if tput setaf 1 &> /dev/null; then
    red=$(tput setaf 1)
    yellow=$(tput setaf 3)
    green=$(tput setaf 2)
    reset=$(tput sgr0)
else
    # If tput fails, fall back to no color
    red=""
    yellow=""
    green=""
    reset=""
fi

codegrade=1

counter=0

# Function to compare floats with precision up to 15 decimal places
compare_floats() {
    awk -v a="$1" -v b="$2" 'BEGIN {
        split(a, a_parts, ".")
        split(b, b_parts, ".")
        
        # Check integer part
        if (a_parts[1] != b_parts[1]) exit 1;
        
        # Compare fractional part up to the first 15 decimals
        a_frac = substr(a_parts[2], 1, 15)
        b_frac = substr(b_parts[2], 1, 15)

        # Compare the first 11 decimals (considered major part)
        a_major = substr(a_frac, 1, 11)
        b_major = substr(b_frac, 1, 11)

        # Compare the last four decimals separately (considered minor part)
        a_minor = substr(a_frac, 12, 4)
        b_minor = substr(b_frac, 12, 4)

        if (a_major != b_major) {
            exit 1;  # Major difference
        } else if (a_minor != b_minor) {
            exit 2;  # Minor difference (last four digits)
        }

        exit 0;  # No difference
    }'
}

# Run pearson to generate sequential output
./pearson "data/128.data" "./data_o/128_seq.data"
./pearson "data/256.data" "./data_o/256_seq.data"
./pearson "data/512.data" "./data_o/512_seq.data"
./pearson "data/1024.data" "./data_o/1024_seq.data"

for thread in 2 4 8 16 32
do
    for size in 128 256 512 1024
    do
        # Run the parallel version
        ./pearson_par "data/$size.data" "./data_o/${size}_par.data" $thread

        # Use cmp to quickly check if there are any differences at all
        if cmp -s "./data_o/${size}_seq.data" "./data_o/${size}_par.data"; then
            echo "Files match for size $size with $thread thread(s)"
        else
            # If cmp finds a difference, proceed to compare floats line by line
            echo "Error: Initial check found differences for size $size with $thread thread(s). Checking details..."
            if [ $codegrade -eq 1 ]; then
                echo "${red}Please run the script locally to find more details about such differences and understand if they are acceptable for this submission.${reset}"
                status=1
                counter=$((counter+1))
                break
            fi

            difference_found=0
            warning_issued=0
            line_num=0

            while IFS= read -r seq_line && IFS= read -r par_line <&3
            do
                line_num=$((line_num + 1))
                compare_floats "$seq_line" "$par_line"
                result=$?

                if [ "$result" -eq 1 ]
                then
                    # Report major differences as errors and print details
                    echo "${red}Error: Mismatch in output data for size $size with $thread thread(s)${reset}"
                    echo "Expected: $seq_line, Got: $par_line (at line $line_num)"
                    status=1
                    difference_found=1
                    counter=$((counter+1))
                    break

                elif [ "$result" -eq 2 ]
                then
                    # Issue warnings for minor differences (last four decimals)
                    if [ "$warning_issued" -eq 0 ]; then
                        echo "${yellow}Warning: Minor differences (last four digits) detected for size $size with $thread thread(s)${reset}"
                        warning_issued=1
                    fi
                fi
            done < "./data_o/${size}_seq.data" 3< "./data_o/${size}_par.data"
        fi

        rm "./data_o/${size}_par.data"
    done
done

if [ $counter -eq 0 ]
then
    echo "${green}Success: All tests passed successfully${reset}"
fi

exit $status