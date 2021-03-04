#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

#include "../include/BloomFilter.h"

struct bloom_filter
{
    uint8_t *array;
    size_t size;
};

BloomFilter create_bloom(size_t kilobytes){

    BloomFilter bloom = (BloomFilter)malloc(sizeof(struct bloom_filter));

    bloom->size = kilobytes;
    bloom->array = (uint8_t *)malloc(sizeof(uint8_t)*1000*kilobytes);
}