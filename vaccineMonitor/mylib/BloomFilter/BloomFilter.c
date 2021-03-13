#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

#include "BloomFilter.h"

#define K 16 /* K is the number of hash functions */

/* Macro to set the nth bit of the filter where n is the index of hash function */
/* n/8: array index */
/* n%8: bit position in array[n/8] */
/* (1 << (n % 8)): shift n positions (000..0010...00) */
/* array[i] | flag: set the bit at the nth position in array[i] */
#define SetBit(array, n) (array[n/8] |= (1 << (n % 8)))

/* Macro to test the bit at the nth position in array[i] */
/* array[i] & flag: test the bit at the nth position in array[i] */
#define TestBit(array, n) (array[n/8] & (1 << (n % 8)))

struct bloom_filter
{
    uint8_t *array; /* array of unsigned integer type with width of exactly 8 bits (1 byte) */
    size_t size;    /* the size of bloom filter in bytes */
};

BloomFilter BloomCreate(size_t kilobytes){

    BloomFilter bloom = (BloomFilter)malloc(sizeof(struct bloom_filter));

    bloom->size = 1000*kilobytes;
    bloom->array = (uint8_t *)malloc(sizeof(uint8_t)*(bloom->size));

    for (int i = 0; i < bloom->size; i++)
        bloom->array[i] = (uint8_t)0;

    return bloom;
}

unsigned long hash_i(unsigned char *str, unsigned int i);

void BloomInsert(BloomFilter bf, void *item){
    /* Set bit for every index that is returned by hash function */
    for (int i = 0; i < K; i++){
        unsigned int hash = hash_i((unsigned char *)item, i)%(bf->size);
        SetBit(bf->array, hash);
    }
}

bool BloomSearch(BloomFilter bf, void *item){
    /* For every index returned by hash function */
    for (int i = 0; i < K; i++){
        unsigned int hash = hash_i((unsigned char *)item, i)%(bf->size);
        /* If at least one of index's bit is 0, item is not in bloom filter, return false */
        if (!TestBit(bf->array, hash))
            return false;
    }
    return true;
}

void BloomDestroy(BloomFilter bf){
    free(bf->array);
    free(bf);
}


/*------------------------------ hash functions ------------------------------*/

/*
This algorithm (k=33) was first reported by dan bernstein many years 
ago in comp.lang.c. 
The magic of number 33 (why it works better than many other constants, 
prime or not) has never been adequately explained.
*/
unsigned long djb2(unsigned char *str) {
	unsigned long hash = 5381;
	int c; 
	while ((c = *str++)) {
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	}
	return hash;
}

/*
This algorithm was created for sdbm (a public-domain reimplementation of ndbm) 
database library. it was found to do well in scrambling bits, causing better 
distribution of the keys and fewer splits. it also happens to be a good 
general hashing function with good distribution. The actual function 
is hash(i) = hash(i - 1) * 65599 + str[i]; what is included below 
is the faster version used in gawk. There is even a faster, duff-device 
version. The magic constant 65599 was picked out of thin air while experimenting 
with different constants, and turns out to be a prime. this is one of the 
algorithms used in berkeley db (see sleepycat) and elsewhere.
*/
unsigned long sdbm(unsigned char *str) {
	unsigned long hash = 0;
	int c;

	while ((c = *str++)) {
		hash = c + (hash << 6) + (hash << 16) - hash;
	}

	return hash;
}

/* 
Return the result of the Kth hash funcation. This function uses djb2 and sdbm.
None of the functions used here is strong for cryptography purposes but they
are good enough for the purpose of the class.

The approach in this function is based on the paper:
https://www.eecs.harvard.edu/~michaelm/postscripts/rsa2008.pdf
*/
unsigned long hash_i(unsigned char *str, unsigned int i){
	return djb2(str) + i*sdbm(str) + i*i;
}