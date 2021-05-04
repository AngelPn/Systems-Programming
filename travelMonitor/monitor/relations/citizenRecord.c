#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "citizenRecord.h"
#include "utils_monitor.h"

struct record
{
    int citizenID;
    char *firstname;
    char *lastname;
    country cntry;
    int age;
};

citizenRecord create_citizen(int citizenID, char *firstname, char *lastname, country cntry, int age){

    if (!(age >= 0 && age <= 120))
        return NULL;
    
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

char *get_citizen_info(citizenRecord rec){
    char *id = concat_int_to_str("", rec->citizenID);
    char *age = concat_int_to_str("", rec->age);
    char *countryName = get_country_name(rec->cntry);

    size_t len = strlen(id) + strlen(rec->firstname) + strlen(rec->firstname) + strlen(countryName) + strlen(age) + 10;
    char *info = (char *)malloc(sizeof(char)*len);
    snprintf(info, len, "%s %s %s %s\nAGE %s\n", id, rec->firstname, rec->lastname, countryName, age);

    free(id); free(age);
    return info;
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