#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/utils.h"
#include "../include/date.h"
#include "../include/virus.h"
#include "../include/citizenRecord.h"

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

int fileParse_and_buildStructs(char *filepath, HashTable *citizens, HashTable *viruses){

	FILE *frecords;
    /* Open the file given from filepath and read it*/
    frecords = fopen(filepath, "r");

    if (frecords == NULL){
        printf("Error: fopen() failed\n");
        exit(EXIT_FAILURE);
    }

	/* Parse the file and build the structs */
    char *line = NULL, *error_line = NULL;
    size_t len = 0;
	citizenRecord citizen = NULL;
	virus v = NULL;
	
    while (getline(&line, &len, frecords) != -1){

         /* Save the line to print in case an input error occurs */
        char *error_line = (char *)malloc(sizeof(char)*(strlen(line)+1));
        strcpy(error_line, line);

		/* Get citizen's information and build citizen's struct (citizenRecord) */
        char *id = strtok(line, " ");
        char *firstname = strtok(NULL, " ");
        char *lastname = strtok(NULL, " ");
        char *country = strtok(NULL, " ");
        char *age = strtok(NULL, " ");

		/* Check if citizen is already in database of citizens (HashTable citizens) */
		/* If not, insert citizen in database of citizens */
        int citizenID = atoi(id);

		if ((citizen = HTSearch(citizens, &citizenID, compare_citizen)) == NULL){
			citizen = create_record(citizenID, firstname, lastname, country, atoi(age));
			HTInsert(citizens, citizen, get_citizenID);
		}

		/* Get the name of virus and check if it is already in database of viruses (HashTable viruses) */
		/* If not, insert virus (v) in database of viruses */
        char *virusName = strtok(NULL, " ");

		if ((v = HTSearch(viruses, virusName, compare_virusName)) == NULL){
			v = create_virus(virusName);
			HTInsert(viruses, v, get_virusName);
		}

		/* Check if citizen is vaccinated to virus or not */
        char *check_vaccinated = strtok(NULL, " ");

		/* If citizen is vaccinated, get the date and insert this information to vaccinated_persons skip list*/
        if (strcmp(check_vaccinated, "YES") == 0){

            char *str_date = strtok(NULL, " \n");
            date d = createdate(str_date);

			vaccinated vaccinated_citizen = create_vaccinated(citizen, d);
            //print_date(d);
        }
        else{
            char *error = strtok(NULL, "\n");
            if (error != NULL){
				printf("ERROR IN RECORD %s\n", error_line);
				continue;
			}
                
        }
        free(error_line);


            
    }



}