#ifndef VIRUSBLOOM_H
#define VIRUSBLOOM_H

#include "BloomFilter.h"
#include "List.h"
#include "HashTable.h"
#include "date.h"

/* "struct virus_bloom_struct" is incomplete struct */
typedef struct virus_bloom_struct *virus_bloom;

/* Creates virus_bloom_struct */
virus_bloom create_virus_bloom(char *virusName, size_t bytes);

/* Returns pointer to virusName of virus_bloom_struct */
void *get_virus_bloomName(void *v);

/* Returns bloom filter of virus */
BloomFilter get_bloom(void *v);

/* Returns the number of accepted/rejected requests between date1 and date2 */
int total_accepted_requests(virus_bloom v, date date1, date date2);
int total_rejected_requests(virus_bloom v, date date1, date date2);

/* Returns the number of accepted/rejected requests between date1 and date2 for country */
int total_accepted_requests_for_country(virus_bloom v, date date1, date date2, char *country);
int total_rejected_requests_for_country(virus_bloom v, date date1, date date2, char *country);

/* Inserts date to the list of accepted/rejected requests for specific virus and country */
void insert_date_accepted_requests(virus_bloom v, date dateTravel, char *countryTo);
void insert_date_rejected_requests(virus_bloom v, date dateTravel, char *countryTo);

/* Updates bloom filter of vires */
void update_BloomFilter(void *v, char *bloom_filter);

/* Compares key with virusName of virus v */
int compare_virus_bloomName(void *key, void *v);

/* Deallocates memory of virus_bloom_struct */
void destroy_virus_bloom(void *v);

#endif