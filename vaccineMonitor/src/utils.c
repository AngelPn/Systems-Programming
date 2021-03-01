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

// int fileParse_and_buildStructs(char *filepath){

// 	FILE *frecords;
//     /*Open the file "citizenRecordsFile.txt" and read it*/
//     frecords = fopen(filepath, "r");
//     if (frecords == NULL){
//         printf("Error: fopen() failed\n");
//         exit(EXIT_FAILURE);
//     }

//     char *line = NULL, *error_line = NULL;
//     size_t len = 0;

//     while (getline(&line, &len, frecords) != -1){
        
//         char *error_line = (char *)malloc(sizeof(char)*(strlen(line)+1));
//         strcpy(error_line, line);
//         char *id = strtok(line, " ");
//         char *firstname = strtok(NULL, " ");
//         char *lastname = strtok(NULL, " ");
//         char *country = strtok(NULL, " ");
//         char *age = strtok(NULL, " ");
//         char *virusName = strtok(NULL, " ");
//         char *vaccinated = strtok(NULL, " ");

//         if (strcmp(vaccinated, "YES") == 0){
//             char *str_date = strtok(NULL, " \n");
//             date d;
//             convert_str_to_date(str_date, &d);
//             //print_date(d);
//         }
//         else{
//             char *error = strtok(NULL, "\n");
//             if (error != NULL)
//                 printf("ERROR IN RECORD %s\n", error_line);
//         }
//         free(error_line);

//         int citizenID = atoi(id);
//         citizenRecord searching_node = HTSearch(HTcitizens, &citizenID, compare_citizen);
//         if (searching_node == NULL){
//             citizenRecord citizen = create_record(citizenID, firstname, lastname, country, atoi(age));
//             HTInsert(&HTcitizens, citizen, get_citizenID);
//             SLInsert(dokimi, citizen, get_citizenID, print_record);
//         }
            
//     }



// }