#!/bin/bash

status=0
red=$(tput setaf 1)
yellow=$(tput setaf 3)
reset=$(tput sgr0)

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

        # Compare the first 13 decimals (considered major part)
        a_major = substr(a_frac, 1, 11)
        b_major = substr(b_frac, 1, 11)

        # Compare the last two decimals separately (considered minor part)
        a_minor = substr(a_frac, 12, 2)
        b_minor = substr(b_frac, 12, 2)

        if (a_major != b_major) {
            exit 1;  # Major difference
        } else if (a_minor != b_minor) {
            exit 2;  # Minor difference (last two digits)
        }

        exit 0;  # No difference
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
                # Issue warnings for minor differences (last two decimals)
                if [ "$warning_issued" -eq 0 ]; then
                    echo "${yellow}Warning: Minor differences (last four digits) detected for size $size and $thread thread(s)${reset}"
                    warning_issued=1
                fi
            fi
        done < "./data_o/${size}_seq.data" 3< "./data_o/${size}_par.data"

        if [ "$difference_found" -eq 0 ]; then
            echo "Files match for size $size with $thread thread(s)"
        fi

        rm "./data_o/${size}_par.data"
    done
done

if [ $counter -eq 0 ]
then
    echo "${green}Success: All tests passed successfully${reset}"
fi

exit $status
