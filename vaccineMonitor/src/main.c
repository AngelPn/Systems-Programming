#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/common_types.h"
#include "../include/List.h"
#include "../include/HashTable.h"
#include "../include/utils.h"
#include "../include/date.h"
#include "citizenRecord.h"

int main(int argc, char **argv){

    int bloom_size; char *filepath = NULL;
    if(!argumentHandling(argc, argv, &bloom_size, &filepath))
        exit(EXIT_FAILURE);

    printf("%s, %d\n", filepath, bloom_size);	

    //List citizens = list_create(destroy_record);
    HashTable HTcitizens = HTCreate(destroy_record);

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
        
        citizenRecord citizen = create_record(atoi(id), firstname, lastname, country, atoi(age));
        //list_insert_next(citizens, list_last(citizens), citizen);

        bucket *searching_node = HTSearch(HTcitizens, id);
        if (searching_node == NULL){
            HTInsert(HTcitizens, id, citizen);
        }
            
    }
    free(line); free(filepath);
    //free(error_line);
    fclose(frecords);

    // printf("Print list\n");
    // list_print(citizens, print_record);
    

    printf("Print Hash table\n");
    HTPrint(HTcitizens, print_record);

    printf("Destroy Hash table\n");
    HTDestroy(HTcitizens);

    //list_destroy(citizens);


	return 0;
}