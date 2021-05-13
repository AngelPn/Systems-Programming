#!/bin/bash

# Check if exactly 3 arguments are provided
if [ "$#" -ne 3 ]; then
    echo "You must enter exactly 3 command line arguments"
    echo "./create_infiles.sh inputFile input_dir numFilesPerDirectory"
    exit 1
fi

# Keep arguments
inputFile=$1
input_dir=$2
numFilesPerDirectory=$3

# Check if inputFile exists
# and if the numFilesPerDirectory is greater than 0
if [ ! -e ${inputFile} ]; then
    echo "Given inputFile does not exist in root directory"
    exit 1
elif [ ${numFilesPerDirectory} -lt "0" ]; then
    echo "Provided numFilesPerDirectory is not greater than 0"
    exit 1
fi

# Create the input dir if it is not already there
if [ ! -e ${input_dir} ]; then
    mkdir ${input_dir}
else
    echo "Input directory already exists. The process will terminate."
    exit 1
fi

# Declare an associative array to keep track of the last written file in country subdir
declare -A assArray

# Go through each line of inputFile
while IFS= read -r line; do

    # Split line string to get the country
    IFS=' ' read -ra words <<< "$line"
    country=${words[3]}

    # Go to input dir and create the subdir for country if it is not already there
    cd ${input_dir}
    if [ ! -e ${country} ]; then
        mkdir ${country}
        assArray[${country}]=1
    fi

    # Go to country dir and store all records of this country to country-file
    cd ${country}
    echo $line >> "$country-${assArray[${country}]}.txt"

    # Distribute round-robin the lines in files
    i=$(( ${assArray[${country}]} + 1 ))
    if [ ${i} -gt ${numFilesPerDirectory} ]; then
        i=1
    fi
    assArray[${country}]=${i}

    cd ../..
done < "$inputFile"
