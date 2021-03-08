#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "citizenRecord.h"

struct record
{
    int citizenID;
    char *firstname;
    char *lastname;
    country cntry;
    int age;
};

citizenRecord create_citizen(int citizenID, char *firstname, char *lastname, country cntry, int age){

    citizenRecord rec = (citizenRecord)malloc(sizeof(struct record));
    rec->citizenID = citizenID;
    rec->age = age;

    rec->firstname = (char *)malloc(sizeof(char)*(strlen(firstname)+1));
    strcpy(rec->firstname, firstname);

    rec->lastname = (char *)malloc(sizeof(char)*(strlen(lastname)+1));
    strcpy(rec->lastname, lastname);

    rec->cntry = cntry;

    // rec->country = (char *)malloc(sizeof(char)*(strlen(country)+1));
    // strcpy(rec->country, country);

    return rec;
}

void *get_citizenID(void *rec){
    citizenRecord crec = rec;
    //printf("get key: %d\n", crec->citizenID);
    return &(crec->citizenID);
}

country get_country(citizenRecord rec){
    return rec->cntry;
}

int get_age(citizenRecord rec){
    return rec->age;
}

void print_citizen(void *rec){
    citizenRecord crec = rec;
    printf("ID: %d, Name: %s, Surname: %s, Age: %d, Country: %s\n", crec->citizenID, crec->firstname, crec->lastname, crec->age, get_country_name(crec->cntry));
}

int compare_citizen(void *key, void *rec){
    int k = *(int *)key;
    printf("key: %d\n", k);
    citizenRecord crec = rec;
    printf("citizenID: %d\n", crec->citizenID);

    if (crec->citizenID == k) return 0;
    else if (crec->citizenID > k) return -1;
    else return 1;
}

void destroy_citizen(void *rec){
    citizenRecord crec = rec;
    free(crec->firstname);
    free(crec->lastname);
    // free(crec->country);
    free(crec);
}