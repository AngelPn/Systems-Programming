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

    return rec;
}

void *get_citizenID(void *rec){
    citizenRecord crec = rec;
    return &(crec->citizenID);
}

country get_country(citizenRecord rec){
    return rec->cntry;
}

int get_age(citizenRecord rec){
    return rec->age;
}

bool cross_check(citizenRecord rec, char *firstname, char *lastname, country cntry, int age){
    if (!strcmp(rec->firstname, firstname) && !strcmp(rec->lastname, lastname) && !strcmp(get_country_name(rec->cntry), get_country_name(cntry)) && rec->age == age)
        return true;
    else return false;
}

void print_citizen(void *rec){
    citizenRecord crec = rec;
    printf("%d %s %s %s %d\n", crec->citizenID, crec->firstname, crec->lastname, (char *)get_country_name(crec->cntry), crec->age);
}

int compare_citizen(void *key, void *rec){
    int k = *(int *)key;
    citizenRecord crec = rec;

    if (crec->citizenID == k) return 0;
    else if (crec->citizenID > k) return -1;
    else return 1;
}

void destroy_citizen(void *rec){
    citizenRecord crec = rec;
    free(crec->firstname);
    free(crec->lastname);
    free(crec);
}