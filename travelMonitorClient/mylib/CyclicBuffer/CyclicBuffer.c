#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "CyclicBuffer.h"

struct buffer
{
    int cyclicBufferSize;
    char **data;
    int dataSize;
    int start;
    int end;
    int count;
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

char *BuffGet(CyclicBuffer buff){
    char *data = buff->data[buff->start];
    buff->start = (buff->start + 1) % (buff->dataSize); /* update start pos */
    (buff->count)--; /* removed an item, decrease count */
    return data;
}

void BuffDestroy(CyclicBuffer buff){
    free(buff);
}