#!/bin/bash

# Check if exactly 4 arguments are provided
if [ "$#" -ne 4 ]; then
    echo "You must enter exactly 4 command line arguments"
    echo "./testFile.sh virusesFile countriesFile numLines duplicatesAllowed"
    exit 1
fi

# Keep arguments
virusesFile=$1
countriesFile=$2
numLines=$3
duplicatesAllowed=$4

# Go to tests directory
cd tests

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

# Create array of IDs: ID is a number from 1 to 9999
# If duplicates are not allowed or numLines < 10000, generate unique IDs
if [ ${duplicatesAllowed} -eq "0" -a ${numLines} -le "10000" ]; then
    ids=($(shuf -i 1-9999 -n $numLines))
# Else generate IDs randomly with duplicates
else
    if [ ${duplicatesAllowed} -eq "0" ]; then
        echo "Given numLines > 10K, so duplicates will be allowed..."
    fi
    # Get shuffled IDs
    shuf_ids=($(shuf -i 1-9999))

    for ((i = 0; i < numLines; )); do
        # x represents how many times the ID will be duplicated
        if [ ${numLines} -gt "10000" -a ${#shuf_ids[@]} -eq "1" ]; then
            x=$((numLines-i))
        else
            x=$((RANDOM % 10 + 1))
        fi
        
        for ((j = 0; j < x; j++)); do
            ids[i]=${shuf_ids[0]}
            i=$(( $i + 1 ))
        done
        # Remove first element in shuf_ids
        shuf_ids=("${shuf_ids[@]:1}")
    done
fi

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

# Create inputFiles
touch inputFiles.txt

function write-in-File(){
    index_virus=$((RANDOM % ${#viruses[@]}))
    virus=${viruses[index_virus]}

    vaccinated=$(yes-no $((RANDOM % 2)))

    if [ "$vaccinated" = "YES" -o $((RANDOM % 2)) -eq "0" ]; then
        # Create random date
        dd=$((RANDOM % 31 + 1))
        mm=$((RANDOM % 12 + 1))
        yyyy=$((RANDOM % 4 + 2018))
        # Make sure both day and month are 2 digits
        if [ $dd -lt "10" ]; then
           dd="0${dd}"
        fi
        if [ $mm -lt "10" ]; then
           mm="0${mm}"
        fi
        date=$dd-$mm-$yyyy
        echo $1 $virus $vaccinated $date >> inputFiles.txt
    else
        echo $1 $virus $vaccinated >> inputFiles.txt
    fi
}

# For the number of lines
for ((i = 0; i < numLines; i++)); do
    # Create citizen's record
    id=${ids[i]}
    firstname=$(rand-str $((RANDOM % 12 + 3)))
    lastname=$(rand-str $((RANDOM % 12 + 3)))

    index_country=$((RANDOM % ${#countries[@]}))
    country=${countries[index_country]}

    age=$((RANDOM % 120 + 1))

    write-in-File "$id $firstname $lastname $country $age"

    # For every time this citizen is duplicated, write in file
    while [ $(( $i + 1 )) -le ${numLines} -a "$id" = "${ids[i+1]}" ]; do
        write-in-File "$id $firstname $lastname $country $age"
        i=$(( $i + 1 ))
    done

done

cd ../