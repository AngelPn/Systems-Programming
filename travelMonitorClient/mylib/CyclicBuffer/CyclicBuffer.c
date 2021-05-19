#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "CyclicBuffer.h"

struct buffer
{
    char **data;
    int start;
    int end;
};

CyclicBuffer BuffCreate(int cyclicBufferSize, char **data, int dataSize){
    CyclicBuffer buff = (CyclicBuffer)malloc(sizeof(struct buffer));

    buff->data = data;
    buff->start = 0;

    if (cyclicBufferSize > dataSize)
        buff->end = dataSize - 1;
    else
        buff->end = cyclicBufferSize - 1;
}

char *BuffGet(CyclicBuffer buff){

}

void BuffDestroy(CyclicBuffer buff){
    free(buff);
}