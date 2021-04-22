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

# Go through each line of inputFile
while IFS= read -r line; do
    # Split line string to get the country
    IFS=' ' read -ra words <<< "$line"
    country=${words[3]}

    # Go to input dir and create the subdir for country if it is not already there
    cd ${input_dir}
    if [ ! -e ${country} ]; then
        mkdir ${country}
    fi

    # Go to country dir and store all records of this country to country-file
    cd ${country}
    echo $line >> "$country-file"

    cd ../..
done < "$inputFile"

# Go to input dir
cd ${input_dir}
# Loop through country directories in input dir
for country in * ; do
    # Go to country dir
    cd ${country}
    # Get the number of records in country file
    num_recs=${#country[@]}

    # For each of the records in country file,
    # distribute round-robin the lines in files 
    i=1
    while IFS= read -r line; do
        echo $line >> "$country-$i.txt"

        i=$(( $i + 1 ))
        if [ ${i} -gt ${numFilesPerDirectory} ]; then
            i=1
        fi
    done < "$country-file"

    # Remove country-file
    rm -f "$country-file"
    
    cd ..
done

cd ..
