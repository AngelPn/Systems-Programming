#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

#include "BloomFilter.h"
#include "ipc.h"

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
    char *array;    /* array of char type with width of 8 bits (1 byte) */
    size_t size;    /* the size of bloom filter in bytes */
};

BloomFilter BloomCreate(size_t bytes){

    BloomFilter bloom = (BloomFilter)malloc(sizeof(struct bloom_filter));

    bloom->size = bytes;
    // bloom->array = (char *)calloc(bloom->size, sizeof(char));
    bloom->array = (char *)malloc((bloom->size)*sizeof(char));
    memset(bloom->array, 0, (bloom->size)*sizeof(char));

    // bloom->array = (uint8_t *)malloc((bloom->size)*sizeof(uint8_t));
    // memset(bloom->array, 0, (bloom->size)*sizeof(uint8_t));
    // for (int i = 0; i < bloom->size; i++)
    //     bloom->array[i] = (uint8_t)0;

    return bloom;
}

char *get_array(BloomFilter bf){
    char *array = (char *)malloc((bf->size)*sizeof(char));
    for (int i = 0; i < bf->size; i++)
        array[i] = bf->array[i];
    return array;
}

void print_bl(BloomFilter bf){
    for (int i = 0; i < bf->size; i++)
        printf("%d", (int)bf->array[i]);
}

void send_bloom_filter(BloomFilter bf, int fd, int bufferSize){
    char arr[bf->size];
    memmove(arr, bf->array, bf->size);
    // printf("\n--------------SEND----------\n");
    // for (int i = 0; i < bf->size; i++)
    //     printf("%d", (int)arr[i]);
    send_data(fd, bufferSize, arr, bf->size);
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
        /* If at least one of indices bit is 0, item is not in bloom filter, return false */
        if (!TestBit(bf->array, hash))
            return false;
    }
    return true;
}

void update_array(BloomFilter bf, char *new_array){
    // printf("update_array\n");
    // printf("\n--------------UPDATE----------\n");
    // for (int i = 0; i < bf->size; i++)
    //     printf("%d", (int)new_array[i]);
    memmove(bf->array, new_array, bf->size);
    // printf("\n--------------UPDATE2----------\n");
    // for (int i = 0; i < bf->size; i++)
    //     printf("%d", (int)bf->array[i]);   
    // for (int i = 0; i < bf->size; i++)
    //     printf("%d", (int)bf->array[i]);
    // strcpy(bf->array, new_array);
    // for (int i = 0; i < bf->size; i++){
    //     bf->array[i] = (bf->array[i] | new_array[i]);
    // }
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