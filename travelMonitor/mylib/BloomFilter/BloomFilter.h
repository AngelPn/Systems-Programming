#ifndef BLOOM_H
#define BLOOM_H

#include <stddef.h>
#include <stdbool.h>

/* "struct bloom_filter" is incomplete struct */
typedef struct bloom_filter *BloomFilter;

/* Creates bloom filter with size of kilobytes */
BloomFilter BloomCreate(size_t bytes);

/* Returns the array of bloom filter */
char *get_array(BloomFilter bf);

/* Inserts in bloom filter the item */
void BloomInsert(BloomFilter bf, void *item);

/* Returns true if item found in bloom filter, else false */
bool BloomSearch(BloomFilter bf, void *item);

/* Updates array of bloom filter given new with bitwise OR */
void update_array(BloomFilter bf, char *new);

/* Destroys bloom filter*/
void BloomDestroy(BloomFilter bf);

#endif