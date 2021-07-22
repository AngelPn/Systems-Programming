# Systems-Programming
Systems Programming Assignments 2021 at DIT - UoA. Each assignment got a score of 100/100 marks.

# [vaccineMonitor](https://github.com/AngelPn/Systems-Programming/tree/main/vaccineMonitor)
The task of [hw1-spring-2021](https://github.com/AngelPn/Systems-Programming/blob/main/vaccineMonitor/hw1-spring-2021.pdf) is the implementation of a program that accepts, processes, records and answers questions about citizens vaccinations. The citizens data come from files produced by [testFile](https://github.com/AngelPn/Systems-Programming/blob/main/vaccineMonitor/testFile.sh) bash script and are stored in **data structures** in main memory. The main data structures are [Bloom Filter](https://github.com/AngelPn/Systems-Programming/tree/main/vaccineMonitor/mylib/BloomFilter) and [Skip List](https://github.com/AngelPn/Systems-Programming/tree/main/vaccineMonitor/mylib/SkipList). Also, [Linked Lists](https://github.com/AngelPn/Systems-Programming/tree/main/vaccineMonitor/mylib/LinkedList) and [Hash Tables](https://github.com/AngelPn/Systems-Programming/tree/main/vaccineMonitor/mylib/HashTable) are used and each of the data structures are of generic type. 

## Application

### Compile and Execute
The application is called `vaccineMonitor` and can be run:
```
$ ./vaccineMonitor -c citizenRecordsFile -b bloomSize
```
* citizenRecordsFile: the name of the file with citizens data produced by bash script.
* bloomSize: the size of Bloom Filter in bytes.

The compilation of the program can be done with `make`.
To run the bash script and produce citizens data input file:
```
$ ./testFile.sh virusesFile countriesFile numLines duplicatesAllowed
```
* [virusesFile](https://github.com/AngelPn/Systems-Programming/blob/main/vaccineMonitor/testFiles/virusesFile), [countriesFile](https://github.com/AngelPn/Systems-Programming/blob/main/vaccineMonitor/testFiles/countriesFile): given files.
* numLines: the number of lines of produced file.
* duplicatedAllowed: if 0, the IDs of citizens are unique, else duplicated are allowed.

### Use the app
When the program finishes reading the `citizenRecordsFile` and storing the citizens data, it shows the message `Enter command: ` and waits for user input. The user can give the following input commands:
- /vaccineStatusBloom citizenID virusName
- /vaccineStatus citizenID virusName
- /vaccineStatus citizenID
- /populationStatus [country] virusName date1 date2
- /popStatusByAge [country] virusName date1 date2
- /insertCitizenRecord citizenID firstName lastName country age virusNames YES/NO [date]
- /vaccineNow citizenID firstName lastName country age virusName
- /list-nonVaccinated-Persons virusName
- /exit

# [travelMonitor](https://github.com/AngelPn/Systems-Programming/tree/main/travelMonitor)
The purpose of [hw2-spring-2021](https://github.com/AngelPn/Systems-Programming/blob/main/travelMonitor/hw2-spring-2021.pdf) is the familiarization with the creation of **processes** using the system calls `fork/exec`, the process communication via **named pipes** and the use of low-level I/O. The [IPC](https://github.com/AngelPn/Systems-Programming/tree/main/travelMonitor/mylib/IPC) is determined by a prototype as the size of buffer for reading and writing via named pipes is specific and given by user.

## Application
The application is called `travelMonitor` and accepts travel requests to other countries from citizens, checks if they are vaccinated for the appropriate virus and accepts or rejects the travel.

### Compile and Execute
To run the program:
```
$ ./travelMonitor -m numMonitors -b bufferSize -s sizeOfBloom -i input_dir
```
* numMonitors: the number of Monitors processes that the program will create.
* bufferSize: the size of buffer for reading and writing via named pipes.
* sizeOfBloom: the size of Bloom Filter.
* input_dir: the name of inpute direcotry that keeps the subdirectories with the files that Monitor processes will handle.

The compilation of the program can be done with `make`.
To run the bash script and produce citizens data `input_dir`:
```
$ ./create_infiles.sh inputFile input_dir numFilesPerDirectory
```
* inputFile: the file with citizens data from `vaccineMonitor` app.
* input_dir: the name of input directory that will hold the subdirectories.
* numFilesPerDirectory: the number of files per subdirectory.

### Use the app
When the program finishes reading the `input_dir` and storing the citizens data, it shows the message `Enter command: ` and waits for user input. The user can give the following input commands:
- /travelRequest citizenID date countryFrom countryTo virusName
- /travelStats virusName date1 date2 [country]
- /addVaccinationRecords country
- /searchVaccinationStatus citizenID
- /exit

# [travelMonitorClient](https://github.com/AngelPn/Systems-Programming/tree/main/travelMonitorClient)
The purpose of [hw3-spring-2021](https://github.com/AngelPn/Systems-Programming/blob/main/travelMonitorClient/hw3-spring-2021.pdf) is the familiarization with the creation of **threads** and **sockets**. The [IPC](https://github.com/AngelPn/Systems-Programming/tree/main/travelMonitorClient/mylib/IPC) is determined by a prototype as the size of buffer for reading and writing via named pipes is specific and given by user.

## Application
The application `travelMonitorClient` is based on `travelMonitor` with the difference that it uses threads instead of processes and the communication between parent process and monitors is done with sockets instead of named pipes. The general idea is the same as the previous assignment, that is: The application accepts travel requests to other countries from citizens, checks if they are vaccinated for the appropriate virus and accepts or rejects the travel.

### Compile and Execute
To run the program:
```
$ ./travelMonitorClient -m numMonitors -b socketbufferSize -c cyclicBufferSize -s sizeOfBloom -i input_dir -t numThreads
```
* socketBufferSize: the size of buffer for reading and writing via sockets.
* cyclicBufferSize: the entries size of [cyclic buffer](https://github.com/AngelPn/Systems-Programming/tree/main/travelMonitorClient/mylib/CyclicBuffer) of `monitorServer`.
* numThreads: the number of threads that the program will create.

The compilation of the program can be done with `make`.

### Use the app
The user input commands are the same as the previous application.
