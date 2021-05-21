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
#include "BloomFilter.h"
#include "monitor.h"
#include "ipc.h"
#include "virus_bloom.h"
#include "date.h"
#include "queries.h"

#define MAX_CMD_LEN 100
#define RED   "\033[1;31m"
#define GRN   "\033[1;32m"
#define YEL   "\033[1;33m"
#define RESET "\033[0m"

void travelRequest(char *args[5], HashTable *monitors, int bloomSize, int bufferSize, int *read_fd, int *write_fd){

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

	/* Check if virus is in hash table of viruses of monitors.
	   If not, create the virus and insert it. */
	if ((v1 = HTSearch(get_monitor_viruses(m1), virusName, compare_virus_bloomName)) == NULL){
		v1 = create_virus_bloom(virusName, bloomSize);
		add_virus(m1, v1);
	}
	if ((v2 = HTSearch(get_monitor_viruses(m2), virusName, compare_virus_bloomName)) == NULL){
		v2 = create_virus_bloom(virusName, bloomSize);
		add_virus(m2, v2);	
	}

	date dateTravel = create_date(str_date);

	/* Search in bloom filter of monitor m1 */
	if (!(BloomSearch(get_bloom(v1), id))){
		printf("REQUEST REJECTED - YOU ARE NOT VACCINATED\n");

		/* Increase counter of rejected requests and save dateTravel */
		increase_rejected_requests(m2, v2, dateTravel, countryTo);
		/* Inform monitor that handles countryTo (m2) that the request was rejected*/
		send_data(read_fd[get_fd_index(m2)], bufferSize, "/request rejected", 0);
	}
	else{
		/* Create the query and write it to pipe */
		char *travelRequest = "/travelRequest";
		char query[strlen(travelRequest) + strlen(id) + strlen(virusName) + 3];
		snprintf(query, sizeof(query), "%s %s %s", travelRequest, id, virusName);
		int fd_index = get_fd_index(m1);
		send_data(read_fd[fd_index], bufferSize, query, 0);

		/* Read from pipe the response */
		char *response = receive_data(read_fd[fd_index], bufferSize);
		if (!strcmp(response, "NO")){
			printf("REQUEST REJECTED - YOU ARE NOT VACCINATED\n");

			/* Increase counter of rejected requests and save dateTravel */
			increase_rejected_requests(m2, v2, dateTravel, countryTo);
			/* Inform monitor that handles countryTo (m2) that the request was rejected*/
			send_data(read_fd[get_fd_index(m2)], bufferSize, "/request rejected", 0);                		
		}
		else{
			char *str_dateVaccinated = response + 4;
			date dateVaccinated = create_date(str_dateVaccinated);
			date date_6_months_later = six_months_later(dateVaccinated);

			if (date_between(dateTravel, dateVaccinated, date_6_months_later)){
				printf("REQUEST ACCEPTED - HAPPY TRAVELS\n");

				/* Increase counter of accepted requests and save dateTravel */
				increase_accepted_requests(m2, v2, dateTravel, countryTo);
				/* Inform monitor that handles countryTo (m2) that the request was accepted*/
				send_data(read_fd[get_fd_index(m2)], bufferSize, "/request accepted", 0);
			}
			else{
				printf("REQUEST REJECTED - YOU WILL NEED ANOTHER VACCINATION BEFORE TRAVEL DATE\n");

				/* Increase counter of rejected requests and save dateTravel */
				increase_rejected_requests(m2, v2, dateTravel, countryTo);
				/* Inform monitor that handles countryTo (m2) that the request was rejected*/
				send_data(read_fd[get_fd_index(m2)], bufferSize, "/request rejected", 0);				
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
						accepted += total_accepted_requests(v, date1, date2);
						rejected += total_rejected_requests(v, date1, date2);
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
			printf(RED "\nERROR: Monitor does not handle this virus\n" RESET);
			return;		
		}
		accepted += total_accepted_requests_for_country(v, date1, date2, country);
		rejected += total_rejected_requests_for_country(v, date1, date2, country);
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
				printf("%s", msg);
				free(msg);
			}
			broke = true;
            break;
        }
		if (broke) break;
    }
}


void execute_queries(HashTable *monitors, int bufferSize, int bloomSize, pid_t *monitors_pids, int *read_fd, int *write_fd, int numActiveMonitors, char *input_dir){
	
	printf(GRN "Enter command:\n" RESET);

	char *line = NULL;
	size_t len = 0;
	bool broke = false;

	while (getline(&line, &len, stdin) != NULL){

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
			travelRequest(args, monitors, bloomSize, bufferSize, read_fd, write_fd);	
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
			List head = NULL;
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
				if (broke){
					broke = false;
					break;
				}
			}
			if (m == NULL){
				printf(RED "\nERROR: Given country not in database\n" RESET);
				return;
			}

			int fd_index = get_fd_index(m);
			pid_t monitor_pid = *((pid_t *)get_monitor_pid(m));
			kill(monitor_pid, SIGUSR1);

			/* Read the bloom filters from the pipe */
			read_bloom_filters(fd_index, m, bufferSize, bloomSize, read_fd);
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
		/* Kill the children and create Log file at the caller function (aggregator) */
		else if (strcmp(query, "/exit") == 0){
			broke = true;
			break;
		}
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
