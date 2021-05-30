#ifndef BUFFER_H
#define BUFFER_H

#include <stdbool.h>

/* struct buffer is incomplete type */
typedef struct buffer *CyclicBuffer;

/* Creates a cyclic buffer of given size */
CyclicBuffer BuffCreate(int cyclicBufferSize);

/* If buff is empty, return true. Else, false */
bool BuffEmpty(CyclicBuffer buff);

/* If buff is full, return true. Else, false */
bool BuffFull(CyclicBuffer buff);

/* If the total number of items that have been inserted to and pulled
   from buffer is equal to 'expected', return true. Else, false */
bool BuffTotal(CyclicBuffer buff, int expected);

/* Increases the total number of items that have been inserted to 
   and pulled from buffer */
void BuffTotal_increase(CyclicBuffer buff);

/* Inserts an item to buffer */
void BuffInsert(CyclicBuffer buff, char *data);

/* Gets data from cyclic buffer */
char *BuffGet(CyclicBuffer buff);

/* Destroys cyclic buffer */
void BuffDestroy(CyclicBuffer buff);

#endif