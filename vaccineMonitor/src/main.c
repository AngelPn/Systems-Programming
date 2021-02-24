#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/common_types.h"
#include "../include/List.h"
#include "citizenRecord.h"

#define MAXSTACK 100

int main(void){
    List citizens = list_create(destroy_record);

    FILE *frecords;
    /*Open the file "citizenRecordsFile.txt" and read it*/
    frecords = fopen("files/citizenRecordsFile.txt","r");
    if (frecords == NULL){
        printf("Error: fopen() failed\n");
        exit(EXIT_FAILURE);
    }

    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, frecords) != -1)
    {
        char *id = strtok(line, " ");
        char *firstname = strtok(NULL, " ");
        char *lastname = strtok(NULL, " ");
        char *country = strtok(NULL, " ");
        char *age = strtok(NULL, " ");
        char *virusName = strtok(NULL, " ");
        char *vaccinated = strtok(NULL, " ");

        if (strcmp(vaccinated, "YES") == 0){
            char *date = strtok(NULL, " \n");
        }
            
        
        citizenRecord citizen = create_record(atoi(id), firstname, lastname, country, atoi(age));
        list_insert_next(citizens, list_first(citizens), citizen);
    }
    free(line);

    fclose(frecords);

    list_print(citizens, print_record);
    list_destroy(citizens);


	return 0;
}