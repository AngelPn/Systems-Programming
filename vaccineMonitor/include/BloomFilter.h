#ifndef BLOOM_H
#define BLOOM_H

#include <stddef.h>
#include <stdbool.h>

/* "struct bloom_filter" is incomplete struct */
typedef struct bloom_filter *BloomFilter;

/* Creates bloom filter */
BloomFilter BloomCreate(size_t kilobytes);
void BloomInsert(BloomFilter bf, void *item);
bool BloomSearch(BloomFilter bf, void *item);
void BloomDestroy(BloomFilter bf);

#endif