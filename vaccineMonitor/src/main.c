#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/common_types.h"
#include "../include/List.h"
#include "../include/HashTable.h"
#include "../include/SkipList.h"
#include "../include/utils.h"
#include "../include/date.h"
#include "citizenRecord.h"

int main(int argc, char **argv){

    int bloom_size; char *filepath = NULL;
    if(!argumentHandling(argc, argv, &bloom_size, &filepath))
        exit(EXIT_FAILURE);

    printf("%s, %d\n", filepath, bloom_size);	

    HashTable HTcitizens = HTCreate(Integer, destroy_record);
    //SkipList dokimi = SLCreate(0.5, NULL);

    FILE *frecords;
    /*Open the file "citizenRecordsFile.txt" and read it*/
    frecords = fopen(filepath, "r");
    if (frecords == NULL){
        printf("Error: fopen() failed\n");
        exit(EXIT_FAILURE);
    }

    char *line = NULL, *error_line = NULL;
    size_t len = 0;

    while (getline(&line, &len, frecords) != -1)
    {
        char *error_line = (char *)malloc(sizeof(char)*(strlen(line)+1));
        strcpy(error_line, line);
        char *id = strtok(line, " ");
        char *firstname = strtok(NULL, " ");
        char *lastname = strtok(NULL, " ");
        char *country = strtok(NULL, " ");
        char *age = strtok(NULL, " ");
        char *virusName = strtok(NULL, " ");
        char *vaccinated = strtok(NULL, " ");

        if (strcmp(vaccinated, "YES") == 0){
            char *str_date = strtok(NULL, " \n");
            date d;
            convert_str_to_date(str_date, &d);
            print_date(d);
        }
        else{
            char *error = strtok(NULL, "\n");
            if (error != NULL)
                printf("ERROR IN RECORD %s\n", error_line);
        }
        free(error_line);

        int citizenID = atoi(id);
        citizenRecord searching_node = HTSearch(HTcitizens, &citizenID, compare_citizen);
        if (searching_node == NULL){
            citizenRecord citizen = create_record(citizenID, firstname, lastname, country, atoi(age));
            HTInsert(&HTcitizens, citizen, get_citizenID);
            //SLInsert(dokimi, citizen, get_citizenID, compare_citizen);
        }
            
    }
    free(line); free(filepath);
    fclose(frecords);
    

    printf("Print Hash table\n");
    HTPrint(HTcitizens, print_record);

    printf("Destroy Hash table\n");
    HTDestroy(HTcitizens);

    printf("Destroy Skip List\n");
    //SLDestroy(dokimi);

	return 0;
}