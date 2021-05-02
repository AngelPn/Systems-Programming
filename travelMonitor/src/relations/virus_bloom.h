#ifndef VIRUSBLOOM_H
#define VIRUSBLOOM_H

#include "BloomFilter.h"
#include "List.h"
#include "date.h"

/* "struct virus_bloom_struct" is incomplete struct */
typedef struct virus_bloom_struct *virus_bloom;

/* Creates virus_bloom_struct */
virus_bloom create_virus_bloom(char *virusName, size_t bytes);

/* Returns pointer to virusName of virus_bloom_struct */
void *get_virus_bloomName(void *v);

/* Returns bloom filter of virus */
BloomFilter get_bloom(void *v);

/* Returns the list of accepted/rejected requests */
List get_accepted(virus_bloom v);
List get_rejected(virus_bloom v);

/* Returns the number of accepted/rejected requests between date1 and date2 */
int accepted_requests(virus_bloom v, date date1, date date2);
int rejected_requests(virus_bloom v, date date1, date date2)

/* Updates bloom filter of vires */
void update_BloomFilter(void *v, char *bloom_filter);

/* Compares key with virusName of virus v */
int compare_virus_bloomName(void *key, void *v);

/* Deallocates memory of virus_bloom_struct */
void destroy_virus_bloom(void *v);

#endif