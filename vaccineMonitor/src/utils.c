#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/utils.h"

/* Does proper error handling and stores variables from command prompt */
int argumentHandling(int argc, char **argv, int *bloomsize, char **filepath){

	for(int i = 1; i < argc; i = i + 2){
		
		if(strcmp(argv[i],"-c") == 0){
			if(i+1 < argc){
                char filecat[50] = "files/";
                strcat(filecat, argv[i+1]);
                strcat(filecat, ".txt");
                *filepath = (char *)malloc(sizeof(char)*(strlen(filecat)+1));
				strcpy(*filepath, filecat);		
			}
			else{
				printf("ERROR: Invalid use of argument\n");				
				return 0;				
			}		
		}
		else if(strcmp(argv[i], "-b") == 0){
			if(i+1 < argc){
			*bloomsize = atoi(argv[i+1]);
				if(*bloomsize <= 0 ){
					printf("ERROR: Invalid argument after -b flag. Number must be positive integer.\n");
					return 0;
				}			
			}
			else{
				printf("ERROR: Invalid use of argument\n");				
				return 0;				
			}						
		} 			
		else{
			printf("ERROR: Invalid use of flags, the process will terminate\n");
			return 0;			
		}
		
	}
    return 1;
}