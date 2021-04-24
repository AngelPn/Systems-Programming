#include <dirent.h>
#include <errno.h>

#include "ipc.h"

char *read_pipe(int fd, int bufferSize){

	/* Read the dataSize */
	int dataSize;
	if ((read(fd, &dataSize, sizeof(int)) == -1) && (errno == EINTR)) 
		return NULL;

	char *data = malloc(sizeof(char)*(dataSize + 1));
	char buffer[bufferSize];
	int read_bytes = 0, total_read_bytes = 0, diff, buf_size;
	while (total_read_bytes < dataSize){
		/* Set the number of bytes to read */
		diff = dataSize - read_bytes;
		buf_size = (diff < bufferSize) ? diff : bufferSize;

		if ((read_bytes = read(fd, buffer, buf_size)) < 0){
			perror("Error in read_pipe");
			exit(1);
		}

		strncpy(data + read_bytes, buffer, buf_size); /* copy bytes from buffer to data */
		total_read_bytes += read_bytes;		
	}

	data[dataSize] = '\0';
	return data;
}

void write_pipe(int fd, int bufferSize, char *data){

	/* Write the dataSize in front of the message */
    int dataSize = strlen(data);
	write(fd, &dataSize, sizeof(int));

	char buffer[bufferSize];
	int written_bytes = 0, total_written_bytes = 0, diff, buf_size;
	while (total_written_bytes < dataSize){
		/* Set the number of bytes to write */
		diff = dataSize - written_bytes;
		buf_size = (diff < bufferSize) ? diff : bufferSize;

		strncpy(buffer, data + written_bytes, buf_size);  /* copy bytes from data to buffer */

		if ((written_bytes = write(fd, buffer, buf_size)) < 0){
			perror("Error in write_pipe");
			exit(1);
		}

		total_written_bytes += written_bytes;
	}
}