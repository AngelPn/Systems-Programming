#ifndef BLOOM_H
#define BLOOM_H

#include <stddef.h>
#include <stdbool.h>

/* "struct bloom_filter" is incomplete struct */
typedef struct bloom_filter *BloomFilter;

/* Creates bloom filter with size of kilobytes */
BloomFilter BloomCreate(size_t bytes);

/* Inserts in bloom filter the item */
void BloomInsert(BloomFilter bf, void *item);

/* Returns true if item found in bloom filter, else false */
bool BloomSearch(BloomFilter bf, void *item);

/* Destroys bloom filter*/
void BloomDestroy(BloomFilter bf);

#endif