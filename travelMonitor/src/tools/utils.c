#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/select.h>

#include "utils.h"
#include "HashTable.h"
#include "monitor.h"

#define RED   "\033[1;31m"
#define GRN   "\033[1;32m"
#define YEL   "\033[1;33m"
#define RESET "\033[0m"

/*  Does proper argument handling and stores variables from command prompt to vars 
    numMonitors, buffersize, bloomsize, input_dir */
int argumentHandling(int argc, char **argv, int *numMonitors, int *buffersize, int *bloomsize, char **input_dir){

	if (argc != 9){
		printf(RED "ERROR: Invalid use of arguments, the process will terminate\n" RESET);
		return 0;			
	}
	
	if (strcmp(argv[1], "-m") == 0){
		if(argv[2] <= 0 ){
			printf(RED "ERROR: Invalid argument after -m flag. Number must be positive integer.\n" RESET);
			return 0;
		}
		else
			*numMonitors = atoi(argv[2]);
	}
	else{
		printf(RED "ERROR: Invalid use of -m flag, the process will terminate\n" RESET);				
		return 0;				
	}

	if (strcmp(argv[3], "-b") == 0){
		if(argv[4] <= 0 ){
			printf(RED "ERROR: Invalid argument after -b flag. Number must be positive integer.\n" RESET);
			return 0;
		}
		else
			*buffersize = atoi(argv[4]);
	}
	else{
		printf(RED "ERROR: Invalid use of -b flag, the process will terminate\n" RESET);				
		return 0;				
	}

	if(strcmp(argv[5], "-s") == 0){
		if(argv[6] <= 0 ){
			printf(RED "ERROR: Invalid argument after -s flag. Number must be positive integer.\n" RESET);
			return 0;
		}
		else
			*bloomsize = atoi(argv[6]);
	} 			
	else{
		printf(RED "ERROR: Invalid use of -s flag, the process will terminate\n" RESET);
		return 0;			
	}

	if (strcmp(argv[7], "-i") == 0){
		*input_dir = (char *)malloc(sizeof(char)*(strlen(argv[8]) + 1));
		strcpy(*input_dir, argv[8]);
	}
	else{
		printf(RED "ERROR: Invalid use of -i flag, the process will terminate\n" RESET);				
		return 0;				
	}

    return 1;
}


char *concat_int_to_string(const char str[], int i){
	char string_i[10];
	snprintf(string_i, 10, "%d", i);
	char *result = (char *)malloc(sizeof(char)*(strlen(str) + strlen(string_i)));
	strcpy(result, str);
	strcat(result, string_i);
	return result;
}

void aggregator(int numMonitors, int bufferSize, int bloomSize, char *input_dir){

    // pid_t pid;
	// pid_t monitors_pids[numMonitors]; /* stores the PIDs of the childs */
    // int reading[numMonitors], writing[numMonitors]; /* stores the file descs for reading and writing */
	// char *names1[numMonitors], *names2[numMonitors]; /* stores the names of named pipes */

	// /* Create a temporary dir with read/write/search permissions for owner, group and others */
    // mkdir("./temp", S_IRWXU | S_IRWXG | S_IRWXO);

	// /* Create numMonitors child processes with fork and named pipes */
    // for (int i = 0; i < numMonitors; i++) {
    //     pid = fork();

    //     /* Store the name of the 2 named pipes */
	// 	names1[i] = concat_int_to_string("./temp/fifo1_", i);
	// 	names2[i] = concat_int_to_string("./temp/fifo2_", i);

    //     if (pid > 0) { /* parent process */
    //         monitors_pids[i] = pid; /* save child's pid */

    //         /* Create named pipes */
    //         if ((mkfifo(names1[i], 0666) == -1) && (errno != EEXIST)) {
	// 			perror("Error creating named pipe");
	// 			exit(EXIT_FAILURE);
    //         }
    //         if ((mkfifo(names2[i], 0666) == -1) && (errno != EEXIST)) {
	// 			perror("Error creating named pipe");
	// 			exit(EXIT_FAILURE);
    //         }
    //     }
	// 	else{ /* child process */
	// 		/* 	Replace the current running process with a new process
	// 			representing the argument list available to the executed program */
    //         // if (execl("../../monitor/Monitor", "Monitor", names1[i], names2[i], itoa(bufferSize), input_dir, "init", NULL) == -1){
	// 		// 	perror("Error in execl");
	// 		// 	exit(EXIT_FAILURE);				
	// 		// }
    //     }
    // }

    // /* Οpen the named pipes and store the file descs */
    // for (int i = 0; i < numMonitors; i++) {
	// 	char *name1 = concat_int_to_string("./temp/fifo1_", i);
	// 	char *name2 = concat_int_to_string("./temp/fifo2_", i);
    //     if ((reading[i] = open(name1, O_RDONLY, 0666)) == -1) {
    //         perror("Error storing file desc in reading array");
    //         exit(EXIT_FAILURE);
    //     }
    //     if ((writing[i] = open(name2, O_WRONLY, 0666)) == -1) {
    //         perror("Error storing file desc in writing array");
    //         exit(EXIT_FAILURE);
    //     }
    //     free(name1);
    //     free(name2);
    // }

    struct dirent *subdir; /* pointer to subdirs*/

    /* Open the input dir */
    DIR *indir = opendir(input_dir);

    if (indir == NULL) {
        perror("Error opening input directory");
        exit(EXIT_FAILURE);
    }

	/* Get the number of subdirs in input dir */
    int numSubdirs = 0;
    while ((subdir = readdir(indir)) != NULL) {
        numSubdirs++;
    }
    rewinddir(indir); /* reset the position of the directory stream */
    
	/* Store country names in countries array, removing . and .. in dirs*/
	numSubdirs -= 2;
	char *countries[numSubdirs];
	int i = 0;
	while ((subdir = readdir(indir)) != NULL){
		if (strcmp(subdir->d_name, ".") == 0 || strcmp(subdir->d_name, "..") == 0)
			continue;
		else{
			countries[i] = strdup(subdir->d_name);
			i++;
		}
	}
	/* Sort countries array alphabetically */
	char *temp = NULL;
	for (int i = 0; i < numSubdirs; i++){
		for (int j = i + 1; j < numSubdirs; j++){
			if (strcmp(countries[i], countries[j]) > 0){
				temp = strdup(countries[i]);
				free(countries[i]); countries[i] = strdup(countries[j]);
				free(countries[j]); countries[j] = strdup(temp);
				free(temp);
			}
		}
	}

	for (int i=0; i < numSubdirs; i++)
		printf("%d. %s\n", i, countries[i]);

    /* Create a hash table to store countries RR alphabetically per monitor */
	HashTable monitors = HTCreate(Integer, destroy_monitor);
	monitor_idx = 0;
	for (int country_idx = 0; country_idx < numSubdirs; country_idx++){
		monitor m = NULL;
		pid_t monitor_pid = monitors_pids[monitor_idx];

		/* Check if monitor with PID is already in hash table of monitors */
		/* If not, insert monitor (m) in hash table of monitors */
		if ((m = HTSearch(monitors, monitor_pid, compare_monitor)) == NULL ){
			m = create_monitor(monitor_pid);
			HTInsert(&(monitors), m, get_monitor_pid);
		}

		add_country(m, countries[country_idx]);
		if ((monitor_idx++) == numMonitors)
			monitor_idx = 0;
	}
}