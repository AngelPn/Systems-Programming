#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/citizenRecord.h"

struct record
{
    int citizenID;
    char *firstname;
    char *lastname;
    char *country;
    int age;
};

citizenRecord create_record(
                int citizenID,
                char *firstname,
                char *lastname,
                char *country,
                int age)
{
    citizenRecord rec = malloc(sizeof(struct record));
    rec->citizenID = citizenID;
    rec->age = age;

    rec->firstname = (char *)malloc(sizeof(char)*(strlen(firstname)+1));
    strcpy(rec->firstname, firstname);

    rec->lastname = (char *)malloc(sizeof(char)*(strlen(lastname)+1));
    strcpy(rec->lastname, lastname);

    rec->country = (char *)malloc(sizeof(char)*(strlen(country)+1));
    strcpy(rec->country, country);

    return rec;
}

void *get_key(citizenRecord rec, char *attrName){
    if (strcmp("citizenID", attrName) == 0)
        return &(rec->citizenID);
    else if(strcmp("firstname", attrName) == 0)
        return rec->firstname;
    else if(strcmp("lastname", attrName) == 0)
        return rec->lastname;
    else if(strcmp("country", attrName) == 0)
        return rec->country;
    else return NULL;
}

void destroy_record(void *rec){
    free(((citizenRecord)rec)->firstname);
    free(((citizenRecord)rec)->lastname);
    free(((citizenRecord)rec)->country);
    free((citizenRecord)rec);
}

void print_record(void *rec){
    printf("ID: %d, Name: %s, Surname: %s, Age: %d, Country: %s\n", ((citizenRecord)rec)->citizenID, ((citizenRecord)rec)->firstname, ((citizenRecord)rec)->lastname, ((citizenRecord)rec)->age, ((citizenRecord)rec)->country);
}