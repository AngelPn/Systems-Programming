#ifndef PIPE_H
#define PIPE_H

/* Read data from <fd> and return it, allocated on the heap */
/* Return NULL on failure */
char *receive_data(int fd, int bufferSize);

/* Write data to the pipe, by writing buffSize bytes each time */
void send_data(int fd, int bufferSize, char *data, int dataSize);

void receive_init(int fd, int *bufferSize, int *bloomSize, char **input_dir);
void send_init(int fd, int bufferSize, int bloomSize, char *input_dir);

#endif