#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "CyclicBuffer.h"

struct buffer
{
    char **data;            /* array of data */
    int dataSize;           /* size of array of data*/
    int cyclicBufferSize;   /* size of cyclic buffer */
    int start;              /* start position of cyclic buffer */
    int end;                /* end position of cyclic buffer */
    int count;              /* counter of items in buffer: determines if buffer is empty or full */
};

CyclicBuffer BuffCreate(int cyclicBufferSize, char **data, int dataSize){
    CyclicBuffer buff = (CyclicBuffer)malloc(sizeof(struct buffer));

    buff->cyclicBufferSize = cyclicBufferSize;
    buff->data = data;
    buff->dataSize = dataSize;
    buff->start = 0;

    if (cyclicBufferSize > dataSize)
        buff->end = dataSize - 1;
    else
        buff->end = cyclicBufferSize - 1;

    buff->count = buff->end + 1;

    return buff;
}

bool BuffEmpty(CyclicBuffer buff){
    if (buff->count > 0)
        return false;
    else return true;
}

bool BuffFull(CyclicBuffer buff){
    if (buff->count >= buff->cyclicBufferSize)
        return true;
    else return false;
}

bool empty_space_in_buff(CyclicBuffer buff){
    if (buff->data[buff->dataSize - 1] != NULL)
        return true;
    else return false;
}

void BuffAdd(CyclicBuffer buff){
    /* Update the end pos in the buffer */
    int removed = buff->cyclicBufferSize - buff->count;

    while(((buff->end + 1) < (buff->dataSize - 1)) && (removed > 0)){
        (buff->end)++;
        (buff->count)++; /* added item, increase count */
        removed--;
    }
}

char *BuffGet(CyclicBuffer buff){
    char *data = buff->data[buff->start];
    buff->data[buff->start] = NULL;
    buff->start = (buff->start + 1) % (buff->dataSize); /* update start pos */
    (buff->count)--; /* removed an item, decrease count */
    return data;
}

void BuffDestroy(CyclicBuffer buff){
    free(buff);
}