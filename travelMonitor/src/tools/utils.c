#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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