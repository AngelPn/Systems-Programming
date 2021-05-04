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

#include "utils.h"
#include "HashTable.h"
#include "BloomFilter.h"
#include "monitor.h"
#include "ipc.h"
#include "virus_bloom.h"
#include "date.h"
#include "virus.h"

#define RED   "\033[1;31m"
#define GRN   "\033[1;32m"
#define YEL   "\033[1;33m"
#define RESET "\033[0m"

/*  Does proper argument handling and stores variables from command prompt to vars 
    numMonitors, buffersize, bloomsize, input_dir */
int argumentHandling(int argc, char **argv, int *numMonitors, int *buffersize, int *bloomsize, char **input_dir){

	if (argc != 9){
		printf(RED "ERROR: Invalid use of arguments, the process will terminate\n" RESET);
		return 0;			
	}
	
	if (strcmp(argv[1], "-m") == 0){
		if(argv[2] <= 0 ){
			printf(RED "ERROR: Invalid argument after -m flag. Number must be positive integer.\n" RESET);
			return 0;
		}
		else
			*numMonitors = atoi(argv[2]);
	}
	else{
		printf(RED "ERROR: Invalid use of -m flag, the process will terminate\n" RESET);				
		return 0;				
	}

	if (strcmp(argv[3], "-b") == 0){
		if(argv[4] <= 0 ){
			printf(RED "ERROR: Invalid argument after -b flag. Number must be positive integer.\n" RESET);
			return 0;
		}
		else
			*buffersize = atoi(argv[4]);
	}
	else{
		printf(RED "ERROR: Invalid use of -b flag, the process will terminate\n" RESET);				
		return 0;				
	}

	if(strcmp(argv[5], "-s") == 0){
		if(argv[6] <= 0 ){
			printf(RED "ERROR: Invalid argument after -s flag. Number must be positive integer.\n" RESET);
			return 0;
		}
		else
			*bloomsize = atoi(argv[6]);
	} 			
	else{
		printf(RED "ERROR: Invalid use of -s flag, the process will terminate\n" RESET);
		return 0;			
	}

	if (strcmp(argv[7], "-i") == 0){
		*input_dir = (char *)malloc(sizeof(char)*(strlen(argv[8]) + 1));
		strcpy(*input_dir, argv[8]);
	}
	else{
		printf(RED "ERROR: Invalid use of -i flag, the process will terminate\n" RESET);				
		return 0;				
	}

    return 1;
}


char *concat_int_to_string(const char str[], int i){
	char string_i[10];
	snprintf(string_i, 10, "%d", i);
	char *result = (char *)malloc(sizeof(char)*(strlen(str) + 1 + sizeof(string_i)));
	strcpy(result, str);
	strcat(result, string_i);
	return result;
}

int cmpstr(const void* p1, const void* p2){
	return strcmp(*(char* const*) p1, *(char* const*) p2);
}

void get_bloom_filters(HashTable *monitors, pid_t *monitors_pids, int numActiveMonitors, int bufferSize, int bloomSize, int *read_fd);
void run_queries(HashTable *monitors, int bufferSize, int bloomSize, pid_t *monitors_pids, int *read_fd, int *write_fd, int numActiveMonitors);

void aggregator(int numMonitors, int bufferSize, int bloomSize, char *input_dir){

    pid_t pid;
	pid_t monitors_pids[numMonitors]; /* stores the PIDs of the childs */
    int read_fd[numMonitors], write_fd[numMonitors]; /* stores the file descs for reading and writing */
	char *names1[numMonitors], *names2[numMonitors]; /* stores the names of named pipes */

	/* Create a temporary dir with read/write/search permissions for owner, group and others */
    mkdir("./tmp", S_IRWXU | S_IRWXG | S_IRWXO);

	/* Create numMonitors child processes with fork and named pipes */
    for (int i = 0; i < numMonitors; i++) {
        pid = fork();

        /* Store the name of the 2 named pipes */
		names1[i] = concat_int_to_string("./tmp/myfifo1_", i);
		names2[i] = concat_int_to_string("./tmp/myfifo2_", i);

        if (pid > 0) { /* parent process */
            monitors_pids[i] = pid; /* save child's pid */

            /* Create named pipes */
            if ((mkfifo(names1[i], 0666) == -1) && (errno != EEXIST)) {
				perror("Error creating named pipe");
				exit(EXIT_FAILURE);
            }
            if ((mkfifo(names2[i], 0666) == -1) && (errno != EEXIST)) {
				perror("Error creating named pipe");
				exit(EXIT_FAILURE);
            }
        }
		else{ /* child process */
            if (execl("Monitor", "Monitor", names1[i], names2[i], NULL) == -1){
				perror("Error in execl");
				exit(EXIT_FAILURE);				
			}
        }
    }

    /* Οpen the named pipes, store the file descs and send init data */
    for (int i = 0; i < numMonitors; i++) {
		char *name1 = concat_int_to_string("./tmp/myfifo1_", i);
		char *name2 = concat_int_to_string("./tmp/myfifo2_", i);
        if ((read_fd[i] = open(name1, O_RDONLY, 0666)) == -1) {
            perror("Error storing file desc in reading array");
            exit(EXIT_FAILURE);
        }
        if ((write_fd[i] = open(name2, O_WRONLY, 0666)) == -1) {
            perror("Error storing file desc in writing array");
            exit(EXIT_FAILURE);
        }
		send_init(write_fd[i], bufferSize, bloomSize, input_dir);
        free(name1);
        free(name2);
    }

    struct dirent *subdir; /* pointer to subdirs*/

    /* Open the input dir */
    DIR *indir;
    if ((indir = opendir(input_dir)) == NULL) {
        perror("Error opening input directory");
        exit(EXIT_FAILURE);
    }

	/* Get the number of subdirs in input dir */
    int numSubdirs = 0;
    while ((subdir = readdir(indir)) != NULL) {
        numSubdirs++;
    }
    rewinddir(indir); /* reset the position of the directory stream */
    
	/* Store country names in countries array, removing . and .. in dirs */
	numSubdirs -= 2;
	char *countries[numSubdirs];
	int i = 0;
	while ((subdir = readdir(indir)) != NULL){
		if (strcmp(subdir->d_name, ".") == 0 || strcmp(subdir->d_name, "..") == 0)
			continue;
		else
			countries[i++] = strdup(subdir->d_name);
	}
    closedir(indir); free(input_dir);

	/* Sort countries array alphabetically using qsort() from the C standard library */
	qsort(countries, numSubdirs, sizeof(char *), cmpstr);

    /* Create a hash table to store countries RR alphabetically per monitor */
	HashTable monitors = HTCreate(Integer, destroy_monitor);
	int monitor_idx = 0, numActiveMonitors = 0;
	for (int country_idx = 0; country_idx < numSubdirs; country_idx++){
		monitor m = NULL;
		pid_t monitor_pid = monitors_pids[monitor_idx];

		/* Check if monitor with PID is already in hash table of monitors */
		/* If not, insert monitor (m) in hash table of monitors */
		if ((m = HTSearch(monitors, &monitor_pid, compare_monitor)) == NULL ){
			m = create_monitor(monitor_pid, monitor_idx);
			HTInsert(&(monitors), m, get_monitor_pid);
		}

		/* Assign the country subdir to monitor */
		char *countryName = countries[country_idx];
		add_country(m, countryName); /* add country in monitor to handle */
		
        send_data(write_fd[monitor_idx], bufferSize, countryName, 0); /* inform the child process through the pipe */
		
		if ((++monitor_idx) == numMonitors){
			monitor_idx = 0; /* reset monitor_idx */
			numActiveMonitors = 1; /* set numActiveMonitors to declare that all monitors are active */
		}
	}

    /* Write 'end' into every pipe to note the end of distribution of countries per monitor */
    for (int i = 0; i < numMonitors; i++){
		send_data(write_fd[i], bufferSize, "end", 0);
	}

	/* Update numActiveMonitors to declare the number of active monitors */
	numActiveMonitors = (numActiveMonitors == 0) ? monitor_idx : numMonitors;

    /* Get bloom filters from monitors */
	get_bloom_filters(&monitors, monitors_pids, numActiveMonitors, bufferSize, bloomSize, read_fd);

	/* Run queries */
	run_queries(&monitors, bufferSize, bloomSize, monitors_pids, read_fd, write_fd, numActiveMonitors);

    /* Send a SIGKILL to the monitors to end them */
    for (int i = 0; i < numMonitors; i++) {
        kill(monitors_pids[i], SIGKILL);
    }
    /* Delete named pipes and remove tmp dir */
    for (int i = 0; i < numMonitors; i++) {
        unlink(names1[i]);
        unlink(names2[i]);
     
        free(names1[i]);
        free(names2[i]);
    }
	rmdir("./tmp");
	
    /* Wait until all the children are dead */
    for (int i = 0; i < numMonitors; i++) {
        wait(&monitors_pids[i]);
    }

	/* Create the LogFiles dir with read/write/search permissions for owner, group and others */
    mkdir("./LogFiles", S_IRWXU | S_IRWXG | S_IRWXO);

	int accepted = 0, rejected = 0;
	monitor m = NULL;
	virus_bloom v = NULL;
	List head1 = NULL, head2 = NULL, m_countries = NULL;

	/* Traverse Hash Table of monitors */
	for (int i = 0; i < HTSize(monitors); i++){
		if ((head1 = get_HTchain(monitors, i)) != NULL){
			for (ListNode node1 = list_first(head1); node1 != NULL; node1 = list_next(head1, node1)){

				/* Create the log_file.xxx for monitor with PID xxx */
				m = list_node_item(head1, node1);
				char *filepath = concat_int_to_string("./LogFiles/log_file.", *((int *)get_monitor_pid(m)));
				if ((mkfifo(filepath, 0666) == -1) && (errno != EEXIST)) {
					perror("Error creating Log file");
					exit(EXIT_FAILURE);
				}

				/* Open the log_file.xxx given from filepath to write */
				FILE *logfile;
				if ((logfile = fopen(filepath, "w")) == NULL){
					perror(RED "Error opening Log file"  RESET);
					exit(EXIT_FAILURE);
				}

				/* Write to log_file.xxx the countries that monitor handles */
				m_countries = get_monitor_countries(m);
				char *countryName = NULL;
				for (ListNode n = list_first(m_countries); n != NULL; n = list_next(m_countries, n)){
					countryName = (char *)list_node_item(m_countries, n);
					fprintf(logfile, "%s\n", countryName);
				}

				/* Traverse the Hash Table of viruses that monitor handles 
				   to count the total number of accepted/rejected requests*/
				HashTable viruses = get_monitor_viruses(m);
				for (int j = 0; j < HTSize(viruses); j++){
					if ((head2 = get_HTchain(viruses, j)) != NULL){
						for (ListNode node2 = list_first(head2); node2 != NULL; node2 = list_next(head2, node2)){

							v = list_node_item(head2, node2);
							accepted += accepted_requests(v, NULL, NULL);
							rejected += rejected_requests(v, NULL, NULL);
						}
					}
				}

				/* Write to log_file.xxx the total number of requests */
				fprintf(logfile, "TOTAL TRAVEL REQUESTS %d\nACCEPTED %d\nREJECTED %d\n", accepted+rejected, accepted, rejected);
				fclose(logfile);
				accepted = rejected = 0;
			}
		}
	}

	/* Deallocate memory */
	HTDestroy(monitors);	
}

void get_bloom_filters(HashTable *monitors, pid_t *monitors_pids, int numActiveMonitors, int bufferSize, int bloomSize, int *read_fd){

    fd_set active, ready; /* represents file descriptor sets for the select function */
    FD_ZERO(&active); /* initializes the file descriptor set 'active' to be the empty set */

	/* Add file descriptors in read_fd array to the file descriptor set 'active' */
    for (int i = 0; i < numActiveMonitors; i++){
		FD_SET(read_fd[i], &active);
	}
	
	monitor m = NULL;
	virus_bloom v = NULL;
	char *virus_name = NULL, *bloom_filter = NULL;
    int counter = 0;

	/* For each of the active monitors, get the incoming bloom filters */
    while (counter < numActiveMonitors){

        /* Find out how many monitors are ready */
		ready = active;
        if (select(FD_SETSIZE, &(ready), NULL, NULL, NULL) < 0){
            perror("Error in select");
            exit(EXIT_FAILURE);
        }

        /* For all the monitors that have already sent bloom filters */
        for (int i = 0; i < numActiveMonitors; i++){

			if (!(FD_ISSET(read_fd[i], &ready)))
				continue;
			// fprintf(stderr, "\n-----------------------i=%d------------------------\n", i);
			/* Get the monitor with specified PID */
			m = HTSearch(*monitors, &(monitors_pids[i]), compare_monitor);

			/* Read the bloom filters from the pipe */
			while (true){
				virus_name = receive_data(read_fd[i], bufferSize);
				if (!strcmp(virus_name, "ready")){
					free(virus_name);
					break;
				}
				// fprintf(stderr, "GBF-%s", virus_name);

				/* Check if virus is already in hash table of viruses of monitor */
				/* If not, insert virus_bloom (v) in hash table of viruses of monitor */
				if ((v = HTSearch(get_monitor_viruses(m), virus_name, compare_virus_bloomName)) == NULL){
					v = create_virus_bloom(virus_name, bloomSize);
					add_virus(m, v);
				}
				// fprintf(stderr, " -AND- ");
				// bloom_filter = receive_data(read_fd[i], bufferSize);
				bloom_filter = receive_BloomFilter(read_fd[i], bufferSize);
				update_BloomFilter(v, bloom_filter);
				// print_bl(get_bloom(v));

				free(bloom_filter);
				free(virus_name);
			}
            counter++;
			FD_CLR(read_fd[i], &active);
        }
    }
}

void travelRequest(char *args[5], HashTable *monitors, int bufferSize, int *read_fd, int *write_fd){

	/* Get data from arguments with right order */
	char *id = args[0];
	char *str_date = args[1];
	char *countryFrom = args[2];
	char *countryTo = args[3];
	char *virusName = args[4];

	/* Find the monitor that handles countryFrom (m1)
	   and the monitor that handles countryTo (m2) */
	monitor m1 = NULL, m2 = NULL, curr = NULL;
	virus_bloom v1 = NULL, v2 = NULL;
	List head = NULL;
	bool broke = false;
	for (int i = 0; i < HTSize(*monitors); i++){
		if ((head = get_HTchain(*monitors, i)) != NULL){
			for (ListNode node = list_first(head); node != NULL; node = list_next(head, node)){
				curr = list_node_item(head, node);
				if (m1 == NULL && handles_country(curr, countryFrom))
					m1 = curr;
				if (m2 == NULL && handles_country(curr, countryTo))
					m2 = curr;
				if (m1 != NULL && m2 != NULL){
					broke = true;
					break;
				}
			}
		}
		if (broke) break;
	}

	if (m1 == NULL){
		printf(RED "\nERROR: Given countryFrom not in database\n" RESET);
		return;
	}
	if (m2 == NULL){
		printf(RED "\nERROR: Given countryTo not in database\n" RESET);
		return;
	}

	/* Check if virus is in hash table of viruses of monitors */
	if ((v1 = HTSearch(get_monitor_viruses(m1), virusName, compare_virus_bloomName)) == NULL){
		printf(RED "\nERROR: Given virusName not in database\n" RESET);
		return;		
	}
	/* If v2 == NULL, the monitor m2 does not check vaccinations for the virus */
	v2 = HTSearch(get_monitor_viruses(m1), virusName, compare_virus_bloomName);

	date dateTravel = create_date(str_date);
	// printf("\n--------------PARENT----------\n");
	// print_bl(get_bloom(v));
	/* Search in bloom filter of monitor m1 */
	if (!(BloomSearch(get_bloom(v1), id))){
		printf("REQUEST REJECTED - YOU ARE NOT VACCINATED\n");

		if (v2 != NULL)
			list_insert_next(get_rejected(v2), NULL, dateTravel);
	}
	else{
		/* Create the query and write it to pipe */
		char *travelRequest = "/travelRequest";
		char query[strlen(travelRequest) + strlen(id) + strlen(virusName) + 3];
		snprintf(query, sizeof(query), "%s %s %s", travelRequest, id, virusName);
		// printf("query: %s\n", query);
		int fd_index = get_fd_index(m1);
		send_data(write_fd[fd_index], bufferSize, query, 0);

		/* Read from pipe the response */
		char *response = receive_data(read_fd[fd_index], bufferSize);
		if (!strcmp(response, "NO")){
			printf("REQUEST REJECTED - YOU ARE NOT VACCINATED\n");

			if (v2 != NULL)
				list_insert_next(get_rejected(v2), NULL, dateTravel);
		}
		else{
			char *str_dateVaccinated = response + 4;
			date dateVaccinated = create_date(str_dateVaccinated);
			date date_6_months_later = six_months_later(dateVaccinated);
			// printf("dateVaccinated: "); print_date(dateVaccinated);
			// printf("dateTravel: "); print_date(dateTravel);
			// printf("date 6 months later: "); print_date(date_6_months_later);
			if (date_between(dateTravel, dateVaccinated, date_6_months_later)){
				printf("REQUEST ACCEPTED - HAPPY TRAVELS\n");

				if (v2 != NULL)
					list_insert_next(get_accepted(v2), NULL, dateTravel);
			}
			else{
				printf("REQUEST REJECTED - YOU WILL NEED ANOTHER VACCINATION BEFORE TRAVEL DATE\n");

				if (v2 != NULL)
					list_insert_next(get_rejected(v2), NULL, dateTravel);
			}
			free(dateVaccinated); free(date_6_months_later);		
		}
		free(response);
	}
}

void travelStats(char *args[4], HashTable *monitors){

	/* Get data from arguments with right order */
	char *virusName = args[0];
	char *country = args[3];
	date date1 = NULL, date2 = NULL;

	/* If date1 exists, make sure date2 exists too */
	if ((date1 = create_date(args[1])) != NULL){
		if((date2 = create_date(args[2])) == NULL){
			printf(RED "\nERROR: date1 must come up with date2 in format: dd-mm-yy\n" RESET);
			free(date1);
			return;
		}
	}

	int accepted = 0, rejected = 0;
	virus_bloom v = NULL;
	monitor m = NULL;
	List head = NULL;

	if (country == NULL){
		for (int i = 0; i < HTSize(*monitors); i++){
			if ((head = get_HTchain(*monitors, i)) != NULL){
				for (ListNode node = list_first(head); node != NULL; node = list_next(head, node)){
					m = list_node_item(head, node);
					if ((v = HTSearch(get_monitor_viruses(m), virusName, compare_virus_bloomName)) != NULL){
						accepted += accepted_requests(v, date1, date2);
						rejected += rejected_requests(v, date1, date2);
					}
				}
			}
		}
	}
	else{
		/* Find the monitor that handles country (m) */
		bool broke = false;
		for (int i = 0; i < HTSize(*monitors); i++){
			if ((head = get_HTchain(*monitors, i)) != NULL){
				for (ListNode node = list_first(head); node != NULL; node = list_next(head, node)){
					m = list_node_item(head, node);
					if (handles_country(m, country)){
						broke = true;
						break;
					}
				}
			}
			if (broke) break;
		}
		if (m == NULL){
			printf(RED "\nERROR: Given country not in database\n" RESET);
			return;
		}

		/* Check if virus is in hash table of viruses of monitors */
		if ((v = HTSearch(get_monitor_viruses(m), virusName, compare_virus_bloomName)) == NULL){
			printf(RED "\nERROR: Given virusName not in database\n" RESET);
			return;		
		}
		accepted += accepted_requests(v, date1, date2);
		rejected += rejected_requests(v, date1, date2);
	}
	printf("TRAVEL REQUESTS %d\n"
			"ACCEPTED %d\n"
			"REJECTED %d\n", accepted + rejected, accepted, rejected);

	free(date1); free(date2);
}

void get_vaccineStatus(HashTable *monitors, pid_t *monitors_pids, int numActiveMonitors, int bufferSize, int *read_fd){

    fd_set active, ready; /* represents file descriptor sets for the select function */
    FD_ZERO(&active); /* initializes the file descriptor set 'active' to be the empty set */

	/* Add file descriptors in read_fd array to the file descriptor set 'active' */
    for (int i = 0; i < numActiveMonitors; i++){
		FD_SET(read_fd[i], &active);
	}

	/* Find out the monitor that responded */
	bool broke = false;
    while (true){
		ready = active;
        if (select(FD_SETSIZE, &(ready), NULL, NULL, NULL) < 0){
            perror("Error in select");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < numActiveMonitors; i++){

			if (!(FD_ISSET(read_fd[i], &ready)))
				continue;

			/* Read from the pipe */
			char *msg = NULL;
			while (true){
				msg = receive_data(read_fd[i], bufferSize);
				if (!strcmp(msg, "end-vaccineStatus")){
					free(msg);
					break;
				}
				// fprintf(stderr, "GBF-%s", virus_name);
				printf("%s", msg);
				free(msg);
			}
			broke = true;
            break;
        }
		if (broke) break;
    }
}



/*  Shows whether a signal raised and awaits handling.
    0 if no signal is pending, else 1. */
static volatile sig_atomic_t sig_intquit_raised;
static volatile sig_atomic_t sig_usr_raised;

/* Functions to handle signals */
void handle_intquit(int signo) { sig_intquit_raised = signo; }
void handle_usr(int signo) { sig_usr_raised = signo; }



void run_queries(HashTable *monitors, int bufferSize, int bloomSize, pid_t *monitors_pids, int *read_fd, int *write_fd, int numActiveMonitors){

	/* Signal sets to handle SIGINT/SIGQUIT and SIGUSR2 respectively */
	struct sigaction act_intquit = {0}, act_usr = {0};

    /* Identify the action to be taken when the signal signo is received */
    // act_intquit.sa_handler = handle_intquit;
    act_usr.sa_handler = handle_usr;

    /* Create a full mask: the signals specified here will be
       blocked during the execution of the sa_handler. */
    sigfillset(&(act_intquit.sa_mask));
	// sigaction(SIGINT, &act_intquit, NULL);
    // sigaction(SIGQUIT, &act_intquit, NULL);

    sigfillset(&(act_usr.sa_mask));
    /* Control specified signals */
    sigaction(SIGUSR2, &act_usr, NULL);

	/* Read input from stdin */
	char *line = NULL;
    size_t len = 0;
	// virus v = NULL;
	bool broke = false;

	printf(GRN "\nEnter command:\n" RESET);

	while (getline(&line, &len, stdin) != -1){

		char *query = strtok(line, " \n");

		if (strcmp(query, "/travelRequest") == 0){
			
			char *args[5];
			for (int i = 0; i < 5; i++){
				args[i] = strtok(NULL, " \n");

				if (args[i] == NULL){
					printf( RED "\nERROR: Invalid input\n" RESET
							YEL "Input format for this command: " RESET
							"/travelRequest citizenID date countryFrom countryTo virusName\n"
							GRN "\nEnter command:\n" RESET);
					broke = true;
					break;
				}
			}
			if (broke){
				broke = false;
				continue;
			}
			travelRequest(args, monitors, bufferSize, read_fd, write_fd);	
		}
		else if (strcmp(query, "/travelStats") == 0){
			
			char *args[4];
			for (int i = 0; i < 3; i++){
				args[i] = strtok(NULL, " \n");

				if (args[i] == NULL){
					printf( RED "\nERROR: Invalid input\n" RESET
							YEL "Input format for this command: " RESET
							"/travelStats virusName date1 date2 [country]\n"
							GRN "\nEnter command:\n" RESET);
					broke = true;
					break;
				}		
			}
			if (broke){
				broke = false;
				continue;
			}
			args[3] = strtok(NULL, " \n");
			travelStats(args, monitors);
		}
		else if (strcmp(query, "/addVaccinationRecords") == 0){

			char *country = strtok(NULL, " \n");
			if (country == NULL){
				printf( RED "\nERROR: Invalid input\n" RESET
						YEL "Input format for this command: " RESET
						"/addVaccinationRecords country\n"
						GRN "\nEnter command:\n" RESET);
				continue;
			}
			/* Find the monitor that handles country (m) */
			monitor m = NULL;
			virus_bloom v = NULL;
			List head = NULL;
			bool broke = false;
			for (int i = 0; i < HTSize(*monitors); i++){
				if ((head = get_HTchain(*monitors, i)) != NULL){
					for (ListNode node = list_first(head); node != NULL; node = list_next(head, node)){
						m = list_node_item(head, node);
						if (handles_country(m, country)){
							broke = true;
							break;
						}
					}
				}
				if (broke) break;
			}
			if (m == NULL){
				printf(RED "\nERROR: Given country not in database\n" RESET);
				return;
			}
			printf("send SIGUSR1\n");

			int fd_index = get_fd_index(m);
			pid_t monitor_pid = *((pid_t *)get_monitor_pid(m));
			kill(monitor_pid, SIGUSR1);

			/* Wait for the signal that informs that the monitor has written in its fd */
			// while (!sig_usr_raised){ }
			// sig_usr_raised = 0; /* reset value */

			/* Read the bloom filters from the pipe */
			while (true){
				char *virus_name = receive_data(read_fd[fd_index], bufferSize);
				if (!strcmp(virus_name, "ready")){
					free(virus_name);
					break;
				}
				// fprintf(stderr, "GBF-%s", virus_name);

				/* Check if virus is already in hash table of viruses of monitor */
				/* If not, insert virus_bloom (v) in hash table of viruses of monitor */
				if ((v = HTSearch(get_monitor_viruses(m), virus_name, compare_virus_bloomName)) == NULL){
					v = create_virus_bloom(virus_name, bloomSize);
					add_virus(m, v);
				}
				// fprintf(stderr, " -AND- ");
				// bloom_filter = receive_data(read_fd[i], bufferSize);
				char *bloom_filter = receive_BloomFilter(read_fd[fd_index], bufferSize);
				update_BloomFilter(v, bloom_filter);
				// print_bl(get_bloom(v));

				free(bloom_filter);
				free(virus_name);
			}
			

		}
		else if (strcmp(query, "/searchVaccinationStatus") == 0){
			char *id = strtok(NULL, " \n");
			if (id == NULL){
				printf( RED "\nERROR: Invalid input\n" RESET
						YEL "Input format for this command: " RESET
						"/searchVaccinationStatus citizenID\n"
						GRN "\nEnter command:\n" RESET);
				continue;
			}
			
			/* Send the query to each of the monitors */
			char searchQuery[strlen(query) + strlen(id) + 2];
			snprintf(searchQuery, sizeof(searchQuery), "%s %s", query, id);
			for (int i = 0; i < numActiveMonitors; i++){
				send_data(write_fd[i], bufferSize, searchQuery, 0);
			}

			get_vaccineStatus(monitors, monitors_pids, numActiveMonitors, bufferSize, read_fd);

		}
		else if (strcmp(query, "/exit") == 0)
			break; /* kill the children and create Log file at the caller function (aggregator) */
		else
			printf( RED "\nERROR: Invalid input\n" RESET
				YEL "Input format for every command:\n" RESET
				"/travelRequest citizenID date countryFrom countryTo virusName\n"
				"/travelStats virusName date1 date2 [country]\n"
				"/addVaccinationRecords country\n"
				"/searchVaccinationStatus citizenID\n"
				"/exit\n");			
		printf(GRN "\nEnter command:\n" RESET);
	}
	free(line);
}