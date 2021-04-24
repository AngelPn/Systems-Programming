#ifndef PIPE_H
#define PIPE_H

/* Read data from <fd> and return it, allocated on the heap */
/* Return NULL on failure */
char *read_pipe(int fd, int bufferSize);

/* Write data to the pipe, by writing buffSize bytes each time */
void write_pipe(int fd, int bufferSize, char *data);

#endif