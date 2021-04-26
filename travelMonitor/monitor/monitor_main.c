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

#include "signal_functions.h"
#include "dataStore.h"

int main(int argc, char **argv){
    fprintf(stderr, "child %d\n", getpid());

    set_signals();

    /* Get arguments */
    int bufferSize = atoi(argv[1]), read_fd, write_fd;
	/* Open named pipes for reading */
	if ((read_fd = open(argv[2], O_WRONLY, 0666)) == -1) {
		perror("Error in opening read_fd in monitor"); exit(EXIT_FAILURE);
	}
    /* Open named pipes for writing */
	if ((write_fd = open(argv[3], O_WRONLY, 0666)) == -1) {
		perror("Error in opening write_fd in monitor"); exit(EXIT_FAILURE);
	}
    char *input_dir = argv[4];

    /* Structures needed for queries */
    dataStore ds;
    create_structs(&ds);


    /* Deallocate memory */
    destroy_structs(&ds);
}