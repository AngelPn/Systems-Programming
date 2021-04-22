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
		*input_dir = (char *)malloc(sizeof(char)*(strlen(argv[2]) + 1));
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

    pid_t pid;
	pid_t monitors_ids[numMonitors]; /* stores the PIDs of the childs */
    int reading[numMonitors], writing[numMonitors]; /* stores the file descs for reading and writing */
	char *names1[numMonitors], *names2[numMonitors]; /* stores the names of named pipes */

	/* Create a temporary dir with read/write/search permissions for owner, group and others */
    mkdir("./temp", S_IRWXU | S_IRWXG | S_IRWXO);

	/* Create numMonitors child processes with fork and named pipes */
    for (int i = 0; i < numMonitors; i++) {
        pid = fork();

        /* Store the name of the 2 named pipes */
		names1[i] = concat_int_to_string("./temp/fifo1_", i);
		names2[i] = concat_int_to_string("./temp/fifo2_", i);

        if (pid > 0) { /* parent process */
            monitors_ids[i] = pid; /* save child's pid */

            /* Create named pipes */
            if ((mkfifo(names1[i], 0666) == -1) && (errno != EEXIST)) {
				perror("Error creating named pipe");
				exit(EXIT_FAILURE);
            }
            if ((mkfifo(names2[i], 0666) == -1) && (errno != EEXIST)) {
				perror("Error creating named pipe");
				exit(EXIT_FAILURE);
            }
        }
		else{ /* child process */
			/* 	Replace the current running process with a new process
				representing the argument list available to the executed program */
            // if (execl("../../monitor/Monitor", "Monitor", names1[i], names2[i], itoa(bufferSize), input_dir, "init", NULL) == -1){
			// 	perror("Error in execl");
			// 	exit(EXIT_FAILURE);				
			// }
        }
    }

    /* Οpen the named pipes and store the file descs */
    for (int i = 0; i < numMonitors; i++) {
		char *name1 = concat_int_to_string("./temp/fifo1_", i);
		char *name2 = concat_int_to_string("./temp/fifo2_", i);
        if ((reading[i] = open(name1, O_RDONLY, 0666)) == -1) {
            perror("Error storing file desc in reading array");
            exit(EXIT_FAILURE);
        }
        if ((writing[i] = open(name2, O_WRONLY, 0666)) == -1) {
            perror("Error storing file desc in writing array");
            exit(EXIT_FAILURE);
        }
        free(name1);
        free(name2);
    }

    struct dirent *subdir; /* pointer to subdirs*/

    /* Open the input dir */
    DIR *indir = opendir(input_dir);

    if (indir == NULL) {
        perror("Error opening input directory");
        exit(EXIT_FAILURE);
    }

	/* Get the number of subdirs in input dir */
    int num_subdirs = 0;
    while ((subdir = readdir(indir)) != NULL) {
        num_subdirs++;
    }
    rewinddir(indir); /* reset the position of the directory stream */
    
	char *countries[num_subdirs]; /* stores country names */
	/* Remove . and .. in dirs */
    for (int i = 0; i < num_subdirs; i++) {
        subdir = readdir(indir);
		if (strcmp(subdir->d_name, ".") && strcmp(subdir->d_name, ".."))
			countries[i] = strdup(subdir->d_name);
		else 
			i--;
    }

	for (int i=0; i < num_subdirs; i++)
		printf("%s\n", countries[i]);
}