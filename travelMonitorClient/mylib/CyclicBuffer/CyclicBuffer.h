#ifndef BUFFER_H
#define BUFFER_H

#include <stdbool.h>

/* struct buffer is incomplete type */
typedef struct buffer *CyclicBuffer;

/* Creates a cyclic buffer of given size */
CyclicBuffer BuffCreate(int cyclicBufferSize, char **data, int dataSize);

/* If buff is empty, return true. Else, false */
bool BuffEmpty(CyclicBuffer buff);

/* If buff is full, return true. Else, false */
bool BuffFull(CyclicBuffer buff);

/* If there is an empty space in buffer, return true. Else false */
bool empty_space_in_buff(CyclicBuffer buff);

/* Moves the position of end pointer to buffer */
void BuffAdd(CyclicBuffer buff);

/* Gets data from cyclic buffer */
char *BuffGet(CyclicBuffer buff);

/* Destroys cyclic buffer */
void BuffDestroy(CyclicBuffer buff);

#endif