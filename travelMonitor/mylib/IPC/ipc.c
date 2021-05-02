#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/select.h>
#include <stdint.h>

#include "ipc.h"

char *receive_data(int fd, int bufferSize){

	/* Read the dataSize */
	int dataSize;
	if ((read(fd, &dataSize, sizeof(int)) == -1) && (errno == EINTR)) 
		return NULL;
	// fprintf(stderr, "dataSize=%d: ", dataSize);

	char *data = (char *)malloc(sizeof(char)*(dataSize + 1));
	char buffer[bufferSize];
	int read_bytes = 0, total_read_bytes = 0, bytes = 0, diff, buf_size;
	while (total_read_bytes < dataSize){
		/* Set the number of bytes to read */
		diff = dataSize - read_bytes;
		if (diff < bufferSize)
			buf_size = diff;
		else buf_size = bufferSize;
		// buf_size = (diff < bufferSize) ? diff : bufferSize;

		if ((read_bytes = read(fd, buffer, buf_size)) < 0){
			perror("Error in read_pipe");
			exit(EXIT_FAILURE);
		}

		/* Copy bytes from buffer to data */
		memcpy(data + bytes, buffer, buf_size);
		bytes = read_bytes;
		total_read_bytes += read_bytes;		
	}

	data[dataSize] = '\0';
	printf("data: %s, read dataSize: %d, total_read: %d\n", data, dataSize, total_read_bytes);
	return data;
}

void send_data(int fd, int bufferSize, char *data, int dataSize){

	/* Write the dataSize in front of the message */
	if (dataSize == 0){
		dataSize = strlen(data);
		
	}
	write(fd, &dataSize, sizeof(int));

	char buffer[bufferSize];
	int written_bytes = 0, total_written_bytes = 0, diff, buf_size;
	while (total_written_bytes < dataSize){
		/* Set the number of bytes to write */
		diff = dataSize - written_bytes;
		if (diff < bufferSize)
			buf_size = diff;
		else buf_size = bufferSize;
		// buf_size = (diff < bufferSize) ? diff : bufferSize;

		/* Copy bytes from data to buffer */
		memcpy(buffer, data + written_bytes, buf_size); 

		if ((written_bytes = write(fd, buffer, buf_size)) < 0){
			perror("Error in write_pipe");
			exit(EXIT_FAILURE);
		}
		total_written_bytes += written_bytes;
	}
	printf("data: %s, write dataSize: %d, total_written: %d\n", data, dataSize, total_written_bytes);
}

void receive_init(int fd, int *bufferSize, int *bloomSize, char **input_dir){
	read(fd, bufferSize, sizeof(int));
	read(fd, bloomSize, sizeof(int));
	*input_dir = strdup(receive_data(fd, *bufferSize));
}

void send_init(int fd, int bufferSize, int bloomSize, char *input_dir){
	write(fd, &bufferSize, sizeof(int));
	write(fd, &bloomSize, sizeof(int));
	send_data(fd, bufferSize, input_dir, 0);
}

char *receive_BloomFilter(int fd, int bufferSize){

	/* Read the dataSize */
	int dataSize;
	if ((read(fd, &dataSize, sizeof(int)) == -1) && (errno == EINTR)) 
		return NULL;
	// fprintf(stderr, "dataSize=%d: ", dataSize);

	char *data = (char *)malloc(sizeof(char)*(dataSize));
	char buffer[bufferSize];
	int read_bytes = 0, total_read_bytes = 0, bytes = 0, diff, buf_size;
	while (total_read_bytes < dataSize){
		/* Set the number of bytes to read */
		diff = dataSize - read_bytes;
		buf_size = (diff < bufferSize) ? diff : bufferSize;

		if ((read_bytes = read(fd, buffer, buf_size)) < 0){
			perror("Error in read_pipe");
			exit(EXIT_FAILURE);
		}

		/* Copy bytes from buffer to data */
		memcpy(data + bytes, buffer, buf_size);
		bytes = read_bytes;
		total_read_bytes += read_bytes;		
	}
	return data;
}