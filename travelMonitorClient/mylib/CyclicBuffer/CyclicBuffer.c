#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "CyclicBuffer.h"

struct buffer
{
    char **data;            /* array of data */
    int cyclicBufferSize;   /* size of cyclic buffer */
    int start;              /* start position of cyclic buffer */
    int end;                /* end position of cyclic buffer */
    int count;              /* counter of items in buffer: determines if buffer is empty or full */
    int total;              /* counter of items that have inserted into buffer */
};

CyclicBuffer BuffCreate(int cyclicBufferSize){
    CyclicBuffer buff = (CyclicBuffer)malloc(sizeof(struct buffer));

    buff->cyclicBufferSize = cyclicBufferSize;

    buff->data = (char **)malloc(sizeof(char *)*cyclicBufferSize);
    for (int i = 0; i < cyclicBufferSize; i++){
        buff->data[i] = NULL;
    }

    buff->start = 0;
    buff->end = -1;
    buff->count = 0;
    buff->total = 0;

    return buff;
}

bool BuffEmpty(CyclicBuffer buff){
    if (buff->count <= 0)
        return true;
    else return false;
}

bool BuffFull(CyclicBuffer buff){
    if (buff->count >= buff->cyclicBufferSize)
        return true;
    else return false;
}

bool BuffTotal(CyclicBuffer buff, int expected){
    if (buff->total == expected)
        return true;
    else return false;
}

void BuffTotal_increase(CyclicBuffer buff){
    (buff->total)++; /* removed an item, increase total count */
}

void BuffInsert(CyclicBuffer buff, char *data){
	buff->end = (buff->end + 1) % (buff->cyclicBufferSize); /* update end pos */
	buff->data[buff->end] = data;
	(buff->count)++; /* added an item, increase count */
}

char *BuffGet(CyclicBuffer buff){
    char *data = buff->data[buff->start];
    buff->data[buff->start] = NULL;
    buff->start = (buff->start + 1) % (buff->cyclicBufferSize); /* update start pos */
    (buff->count)--; /* removed an item, decrease count */
    return data;
}

void BuffDestroy(CyclicBuffer buff){
    free(buff->data);
    free(buff);
}