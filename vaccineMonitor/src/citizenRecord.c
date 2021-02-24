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
    citizenRecord crec = rec;
    free(crec->firstname);
    free(crec->lastname);
    free(crec->country);
    free(crec);
}

void print_record(void *rec){
    citizenRecord crec = rec;
    printf("ID: %d, Name: %s, Surname: %s, Age: %d, Country: %s\n", crec->citizenID, crec->firstname, crec->lastname, crec->age, crec->country);
}