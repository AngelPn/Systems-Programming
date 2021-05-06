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
#include "queries.h"

#define RED   "\033[1;31m"
#define GRN   "\033[1;32m"
#define YEL   "\033[1;33m"
#define RESET "\033[0m"

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
	v2 = HTSearch(get_monitor_viruses(m2), virusName, compare_virus_bloomName);

	date dateTravel = create_date(str_date);
	// printf("\n--------------PARENT----------\n");
	// print_bl(get_bloom(v));
	/* Search in bloom filter of monitor m1 */
	if (!(BloomSearch(get_bloom(v1), id))){
		printf("REQUEST REJECTED - YOU ARE NOT VACCINATED\n");

		if (v2 != NULL){
			list_insert_next(get_rejected(v2), NULL, dateTravel); /* save dateTravel as rejected request*/
			increase_rejected_requests(m2); /* increase counter of rejected requests*/
            /* Inform monitor that handles countryTo (m2) that the request was rejected*/
            send_data(write_fd[get_fd_index(m2)], bufferSize, "/request rejected", 0);
		}	
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

			if (v2 != NULL){
				list_insert_next(get_rejected(v2), NULL, dateTravel);
				increase_rejected_requests(m2);
                /* Inform monitor that handles countryTo (m2) that the request was rejected*/
                send_data(write_fd[get_fd_index(m2)], bufferSize, "/request rejected", 0);                		
			}
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

				if (v2 != NULL){
					list_insert_next(get_accepted(v2), NULL, dateTravel);
					increase_accepted_requests(m2);
                    /* Inform monitor that handles countryTo (m2) that the request was accepted*/
                    send_data(write_fd[get_fd_index(m2)], bufferSize, "/request accepted", 0);
				}
			}
			else{
				printf("REQUEST REJECTED - YOU WILL NEED ANOTHER VACCINATION BEFORE TRAVEL DATE\n");

				if (v2 != NULL){
					list_insert_next(get_rejected(v2), NULL, dateTravel);
					increase_rejected_requests(m2);
                    /* Inform monitor that handles countryTo (m2) that the request was rejected*/
                    send_data(write_fd[get_fd_index(m2)], bufferSize, "/request rejected", 0);				
				}
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
static volatile sig_atomic_t sig_chld_raised;

/* Functions to handle signals */
void handle_intquit(int signo) { sig_intquit_raised = signo; }
void handle_usr(int signo) { sig_usr_raised = signo; }
void handle_chld(int signo) { sig_chld_raised = signo; }



void run_queries(HashTable *monitors, int bufferSize, int bloomSize, pid_t *monitors_pids, int *read_fd, int *write_fd, int numActiveMonitors, char *input_dir){

	/* Signal sets to handle SIGINT/SIGQUIT, SIGUSR2 and SIGCHLD respectively */
	struct sigaction act_intquit = {0}, act_usr = {0}, act_chld = {0};

    /* Identify the action to be taken when the signal signo is received */
    act_intquit.sa_handler = handle_intquit;
    act_usr.sa_handler = handle_usr;
    act_chld.sa_handler = handle_chld;

    /* Create a full mask: the signals specified here will be
       blocked during the execution of the sa_handler. */
    sigfillset(&(act_intquit.sa_mask));
    sigfillset(&(act_usr.sa_mask));
    sigfillset(&(act_chld.sa_mask));
    
    /* Control specified signals */
	sigaction(SIGINT, &act_intquit, NULL);
    sigaction(SIGQUIT, &act_intquit, NULL);    
    sigaction(SIGUSR2, &act_usr, NULL);
    sigaction(SIGCHLD, &act_chld, NULL);

	/* Read input from stdin */
	char *line = NULL;
    size_t len = 0;
	// virus v = NULL;
	bool broke = false;

	printf(GRN "\nEnter command:\n" RESET);

	while (getline(&line, &len, stdin) != -1){

		char *query = strtok(line, " \n");

        /* If a child process is dead, replace it */
        if (sig_chld_raised){
			printf("SIGCHLD raised\n");
			reborn_child(monitors, monitors_pids, bufferSize, bloomSize, read_fd, write_fd, numActiveMonitors, input_dir);
			sig_chld_raised = 0;
			continue;
        }

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

		if (sig_intquit_raised){
			break; /* do the same as '/exit' */
		}
		printf(GRN "\nEnter command:\n" RESET);
	}
	free(line);
}