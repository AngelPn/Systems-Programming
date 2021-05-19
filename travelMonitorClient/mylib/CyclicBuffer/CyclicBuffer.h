#ifndef BUFFER_H
#define BUFFER_H

/* struct buffer is incomplete type */
typedef struct buffer *CyclicBuffer;

/* Creates a cyclic buffer of given size */
CyclicBuffer BuffCreate(int cyclicBufferSize, char **data, int dataSize);

// /* Inserts data to cyclic buffer */
// void BuffInsert(CyclicBuffer buff, char *data);

/* Gets data from cyclic buffer */
char *BuffGet(CyclicBuffer buff);

/* Destroys cyclic buffer */
void BuffDestroy(CyclicBuffer buff);

#endif