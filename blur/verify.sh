#!/bin/bash

status=0
red=$(tput setaf 1)
green=$(tput setaf 2)
reset=$(tput sgr0)

counter=0

for thread in 1 2 4 8 16 32
do
    for image in im1 im2 im3 im4
    do
        ./blur_par 15 "data/${image}.ppm" "./data_o/${image}_par.ppm" $thread

        if ! cmp -s "./data_o/${image}_seq.ppm" "./data_o/${image}_par.ppm"
        then
            echo "${red}Error: Mismatch in output data for image ${image}.ppm with ${thread} thread(s)${reset}"
            counter=$((counter+1))
            status=1
        fi

        rm "./data_o/${image}_par.ppm"
    done
done

if [ $counter -eq 0 ]
then
    echo "${green}Success: All tests passed successfully${reset}"
fi

exit $status