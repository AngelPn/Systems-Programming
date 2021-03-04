#ifndef BLOOM_H
#define BLOOM_H

#include <stddef.h>
// #include <stdbool.h>

/* "struct bloom_filter" is incomplete struct */
typedef struct bloom_filter *BloomFilter;

/* Creates bloom filter */
BloomFilter create_bloom(size_t kilobytes);

void BloomInsert(BloomFilter bloom, void *item);

#endif