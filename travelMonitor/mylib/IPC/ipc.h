#ifndef PIPE_H
#define PIPE_H

/* Read data from <fd> and return it, allocated on the heap */
/* Return NULL on failure */
char *receive_data(int fd, int bufferSize);

/* Write data to the pipe, by writing bufferSize bytes each time */
void send_data(int fd, int bufferSize, char *data, int dataSize);

/* Receives/Sends the bufferSize, bloomSize and input_dir */
void receive_init(int fd, int *bufferSize, int *bloomSize, char **input_dir);
void send_init(int fd, int bufferSize, int bloomSize, char *input_dir);

/* Receives bloom filter */
char *receive_BloomFilter(int fd, int bufferSize);

#endif