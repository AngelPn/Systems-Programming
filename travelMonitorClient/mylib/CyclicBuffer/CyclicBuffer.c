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

void BuffInsert(CyclicBuffer buff, char *data){
	buff->end = (buff->end + 1) % (buff->cyclicBufferSize); /* update end pos */
    // printf("BuffInsert pos = %d: %s\n", buff->end, data);
	buff->data[buff->end] = data;
	(buff->count)++; /* added an item, increase count */
}

char *BuffGet(CyclicBuffer buff){
    char *data = buff->data[buff->start];
    // printf("BuffGet pos = %d: %s\n", buff->start, data);
    buff->data[buff->start] = NULL;
    buff->start = (buff->start + 1) % (buff->cyclicBufferSize); /* update start pos */
    (buff->count)--; /* removed an item, decrease count */
    return data;
}

void BuffRemoved(CyclicBuffer buff){
    (buff->total)++; /* removed an item, decrease count */
    // printf("Buff->total = %d\n", buff->total);
}

void BuffNull(CyclicBuffer buff, char *method){
    printf("%s : ", method);
    for (int i = 0; i < buff->cyclicBufferSize; i++){
        if (buff->data[i] == NULL)
            printf("%d ", i);
    }
    printf("\n");
}

void BuffDestroy(CyclicBuffer buff){
    free(buff->data);
    free(buff);
}