#!/bin/bash

# Check if exactly 4 arguments are provided
if [ "$#" -ne 4 ]; then
    echo "You must enter exactly 4 command line arguments"
    echo "./testFile.sh virusesFile countriesFile numLines duplicatesAllowed"
    exit 1
fi

# Keep arguments
virusesFile=tests/$1
countriesFile=tests/$2
numLines=$3
duplicatesAllowed=$4

# Check if virusesFile and countriesFile exists
# and if the two numbers given are greater than 0
if [ ! -e ${virusesFile} -o ! -e ${countriesFile} ]; then
    echo "Given files does not exist in tests directory"
    exit 1
elif [ ${numLines} -lt "0" -o ${duplicatesAllowed} -lt "0" ]; then
    echo "Provided numbers are not greater than 0"
    exit 1
fi

# Create arrays with the viruses and countries
viruses+=($(cat ${virusesFile}))
countries+=($(cat ${countriesFile}))

# Return random string of letters of length given in argument
function rand-str() {
    # -dc: delete all except given set
    tr -dc a-z </dev/urandom | head -c $1
}

# Return randomly YES or NO
function yes-no() {
    if [ $1 -eq "0" ]; then
        echo "YES"
    else
        echo "NO"
    fi
}

# Go to test directory
cd tests

# Create inputFiles
touch inputFiles.txt

# For the number of lines
for ((i = 0; i < numLines; i++)); do
    # Create citizen's record
    id=$((RANDOM % 9999 + 1))
    firstname=$(rand-str $((RANDOM % 12 + 3)))
    lastname=$(rand-str $((RANDOM % 12 + 3)))

    index_country=$((RANDOM % ${#countries[@]}))
    country=${countries[index_country]}

    age=$((RANDOM % 120 + 1))

    index_virus=$((RANDOM % ${#viruses[@]}))
    virus=${countries[index-virus]}

    vaccinated=$(yes-no $((RANDOM % 2)))

    if [ "$vaccinated" = "YES" -o $((RANDOM % 2)) -eq "0" ]; then
        # Create random date
        dd=$((RANDOM % 31 + 1))
        mm=$((RANDOM % 12 + 1))
        yyyy=$((RANDOM % 4 + 2018))
        date=$dd-$mm-$yyyy
        echo $id $firstname $lastname $country $age $vaccinated $date >> inputFiles.txt
    else
        echo $id $firstname $lastname $country $age $vaccinated >> inputFiles.txt
    fi
done

cd ../