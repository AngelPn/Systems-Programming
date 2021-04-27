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
#include "ipc.h"
#include "country.h"
#include "utils_monitor.h"

int main(int argc, char **argv){
    fprintf(stderr, "child %d\n", getpid());

    set_signals();

    /* Get arguments */
    int bufferSize = atoi(argv[1]), bloomSize = atoi(argv[2]), read_fd, write_fd;
	/* Open named pipes for writing */
	if ((write_fd = open(argv[3], O_WRONLY, 0666)) == -1) {
		perror("Error in opening write_fd in monitor"); exit(EXIT_FAILURE);
	}
    /* Open named pipes for reading */
	if ((read_fd = open(argv[4], O_RDONLY, 0666)) == -1) {
		perror("Error in opening read_fd in monitor"); exit(EXIT_FAILURE);
	}
    char *input_dir = argv[5];
    printf("(monitor_main)Opened named pipes and stored the file descs %s: %d, %s: %d\n", argv[2], write_fd,argv[3],  read_fd);

    /* Structures needed for queries */
    dataStore ds;
    create_structs(&ds);

    country c;
	char *country_name;
	/* Read the dirs from the pipe */
	while (true) {
		country_name = receive_data(read_fd, bufferSize);
		if (!strcmp(country_name, "end")) 
			break;
		fprintf(stderr, "RCV-%s\n", country_name);
		/* Check if country is already in hash table of countries */
		/* If not, insert country (c) in hash table of countries */
		if ((c = HTSearch(ds.countries, country_name, compare_countries)) == NULL ){
			c = create_country(country_name);
			HTInsert(&(ds.countries), c, get_country_name);
		}
	}



	/* If at least one country is assigned to monitor */
	if (HTSize(ds.countries)){
		fileParse_and_buildStructs(input_dir, bloomSize, &ds);

		/* Send bloom filters to parent process */
	}
	/* If no countries are assigned, wait for SIGKILL*/
	else{
		while(true);
	}

    printf("(monitor_main)Print HT for child %d\n", getpid());
    print_ht_countries(&ds);
	print_ht_citizens(&ds);

    /* Deallocate memory */
    destroy_structs(&ds);
}