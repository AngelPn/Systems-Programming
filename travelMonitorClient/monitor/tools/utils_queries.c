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
#include <signal.h>
#include <sys/select.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "utils_queries.h"
#include "virus.h"
#include "country.h"
#include "ipc.h"
#include "BloomFilter.h"
#include "CyclicBuffer.h"

#define RED   "\033[1;31m"
#define GRN   "\033[1;32m"
#define YEL   "\033[1;33m"
#define RESET "\033[0m"

extern pthread_mutex_t mtx;
extern pthread_cond_t nonempty;
extern pthread_cond_t nonfull;
extern dataStore ds;

void argsHandling(int argc, char **argv, int *port, int *numThreads, int *socketBufferSize, int *cyclicBufferSize, int *bloomsize, char ***paths, int *paths_len){

	if (strcmp(argv[1], "-p") == 0){
		if(argv[2] <= 0 ){
			printf(RED "ERROR: Invalid argument after -p flag. Number must be positive integer.\n" RESET);
			exit(EXIT_FAILURE);
		}
		else
			*port = atoi(argv[2]);
	}
	else{
		printf(RED "ERROR: Invalid use of -p flag, the process will terminate\n" RESET);				
		exit(EXIT_FAILURE);				
	}
	
	if(strcmp(argv[3], "-t") == 0){
		if(argv[4] <= 0 ){
			printf(RED "ERROR: Invalid argument after -t flag. Number must be positive integer.\n" RESET);
			exit(EXIT_FAILURE);
		}
		else
			*numThreads = atoi(argv[4]);
	} 			
	else{
		printf(RED "ERROR: Invalid use of -t flag, the process will terminate\n" RESET);
		exit(EXIT_FAILURE);			
	}

	if (strcmp(argv[5], "-b") == 0){
		if(argv[6] <= 0 ){
			printf(RED "ERROR: Invalid argument after -b flag. Number must be positive integer.\n" RESET);
			exit(EXIT_FAILURE);
		}
		else
			*socketBufferSize = atoi(argv[6]);
	}
	else{
		printf(RED "ERROR: Invalid use of -b flag, the process will terminate\n" RESET);				
		exit(EXIT_FAILURE);			
	}

	if (strcmp(argv[7], "-c") == 0){
		if(argv[8] <= 0 ){
			printf(RED "ERROR: Invalid argument after -c flag. Number must be positive integer.\n" RESET);
			exit(EXIT_FAILURE);
		}
		else
			*cyclicBufferSize = atoi(argv[8]);
	}
	else{
		printf(RED "ERROR: Invalid use of -c flag, the process will terminate\n" RESET);				
		exit(EXIT_FAILURE);				
	}

	if(strcmp(argv[9], "-s") == 0){
		if(argv[10] <= 0 ){
			printf(RED "ERROR: Invalid argument after -s flag. Number must be positive integer.\n" RESET);
			exit(EXIT_FAILURE);
		}
		else
			*bloomsize = atoi(argv[10]);
	} 			
	else{
		printf(RED "ERROR: Invalid use of -s flag, the process will terminate\n" RESET);
		exit(EXIT_FAILURE);			
	}

	int j = 11;
	*paths_len = argc - 11;
	*paths = (char **)malloc(sizeof(char *)*(*paths_len));
	for (int i = 0; i < (*paths_len); i++){
		(*paths)[i] = (char *)malloc(sizeof(char)*(strlen(argv[j]) + 1));
		strcpy((*paths)[i], argv[j]);
		j++;
	}
}

List get_filepaths(char **subdirPaths, int subdirPaths_len){

	List filepaths = list_create(NULL);

	for (int i = 0; i < subdirPaths_len; i++){

		/* Open the country subdir */
		DIR *countryDir;
		if ((countryDir = opendir(subdirPaths[i])) == NULL) {
			perror(RED "Error opening country's subdirectory" RESET);
			exit(EXIT_FAILURE);
		}
		// printf("subdirpath: %s\n", subdirPaths[i]);
		/* Traverse all the files in the country subdir */
		struct dirent *file;
		while ((file = readdir(countryDir)) != NULL) {
			
			/* Ignore the . and .. files */
			if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0)
				continue;

			/* Add the filename to parsed files, if it is not already parsed */
			char *filename = strdup(file->d_name);
			if (is_parsed(&ds, filename)){
				free(filename);
				continue;
			}
			list_insert_next(ds.parsed_files, NULL, filename);

			/* Create the path of file */
			char filePath[strlen(subdirPaths[i]) + strlen(filename) + 2];
			snprintf(filePath, sizeof(filePath), "%s/%s", subdirPaths[i], filename);
			

			list_insert_next(filepaths, NULL, strdup(filePath));
		}		

		/* Close the country subdir*/
		closedir(countryDir);

		free(subdirPaths[i]);
	}

	return filepaths;
}

void insertCitizen(char *args[8], int bytes, bool fileparse);

/* Does file parsing and builds structs in dataStore */
void *fileParse_and_buildStructs(void *buff){

	CyclicBuffer buffer = buff;

	while (true){

		pthread_mutex_lock(&mtx); /* shared data area */

		/* If buffer is empty, wait for signal nonempty */
		while (BuffEmpty(buffer)){
			pthread_cond_wait(&nonempty, &mtx);
		}

		/* Get filepath from buffer */
		char *filePath = BuffGet(buffer);
		// printf("BuffGet: %s\n", filePath);

		pthread_mutex_unlock(&mtx);

		/* The buffer is not full anymore */
		pthread_cond_signal(&nonfull);

		/* Open the file given from filePath and read it */
		FILE *frecords;
		if ((frecords = fopen(filePath, "r")) == NULL){
			perror(RED "Error opening country's file"  RESET);
			exit(EXIT_FAILURE);
		}

		/* Parse the file and build the structs */
		char *line = NULL;
		size_t len = 0;
		
		while (getline(&line, &len, frecords) != -1){
			char *args[8];
			args[0] = strtok(line, " ");
			for (int i = 1; i < 8; i++)
				args[i] = strtok(NULL, " \n");

			pthread_mutex_lock(&mtx); /* shared data area */
			insertCitizen(args, ds.bloomSize, true);
			pthread_mutex_unlock(&mtx);   
		}

		free(line);
		free(filePath);
		fclose(frecords);
	}
}

char *concat_int_to_str(const char str[], int i){
	char string_i[10];
	snprintf(string_i, 10, "%d", i);
	char *result = (char *)malloc(sizeof(char)*(strlen(str) + 1 + sizeof(string_i)));
	strcpy(result, str);
	strcat(result, string_i);
	return result;
}


void send_bloomFilters(int conn_fd, int bufferSize, int bloomSize){

	virus v = NULL;

	/* For each of monitor's virus, send virus name and bloom filter of virus */
	List head = NULL;
	for (int i = 0; i < HTSize(ds.viruses); i++){
		head = get_HTchain(ds.viruses, i);
		if(head != NULL){
			for (ListNode node = list_first(head); node != NULL; node = list_next(head, node)){
				v = list_node_item(head, node);
				char *virusName =  (char *)get_virusName(v);
				send_data(conn_fd, bufferSize, virusName, 0);
				send_data(conn_fd, bufferSize, get_array(get_filter(v)), bloomSize);
			}
		}
	}
	/* Inform the parent that monitor is ready to run queries */
	send_data(conn_fd, bufferSize, "ready", 0);
}


void insertCitizen(char *args[8], int bytes, bool fileparse){

	/* Get data from arguments with right order */
	char *id = args[0];
	char *firstname = args[1];
	char *lastname = args[2];
	char *country_name = args[3];
	char *age = args[4];
	char *virusName = args[5];
	char *check_vaccinated = args[6];
	char *str_date = args[7];
	
	if (strcmp(check_vaccinated, "NO") == 0 && str_date != NULL)
		return;
	else{
		country c = NULL;
		citizenRecord citizen = NULL;
		vaccinated vaccinated_citizen = NULL;
		virus v = NULL;
		
		/* Check if country is already in hash table of countries */
		/* If not, insert country (c) in hash table of countries */
		if ((c = HTSearch(ds.countries, country_name, compare_countries)) == NULL ){
			c = create_country(country_name);
			HTInsert(&(ds.countries), c, get_country_name);
		}

		/* Check if citizen is already in hash table of citizens */
		/* If not, insert citizen in hash table of citizens */
		int citizenID = atoi(id);
		if ((citizen = HTSearch(ds.citizens, &citizenID, compare_citizen)) == NULL){
			if ((citizen = create_citizen(citizenID, firstname, lastname, c, atoi(age))) == NULL)
				return;
			else
				HTInsert(&(ds.citizens), citizen, get_citizenID);
		}
		/* If citizen is already in hash tbale of citizens, cross-check given data */
		else{
			if (!cross_check(citizen, firstname, lastname, c, atoi(age)))
				return;
		}

		/* Check if virus is already in hash table of viruses */
		/* If not, insert virus (v) in hash table of viruses */
		if ((v = HTSearch(ds.viruses, virusName, compare_virusName)) == NULL){
			v = create_virus(virusName, bytes);
			HTInsert(&(ds.viruses), v, get_virusName);
		}

		/* If citizen is vaccinated, insert citizen to vaccinated_persons skip list and bloom filter */
		if (strcmp(check_vaccinated, "YES") == 0 || strcmp(check_vaccinated, "/vaccinateNow") == 0){

			/* Make sure vaccinated citizen is not already in skip list */
			if ((vaccinated_citizen = SLSearch(get_vaccinated_persons(v), &citizenID, compare_vaccinated)) != NULL)
				return;
			else{
				/* Search first if citizen is in not_vaccinated_persons to remove it or dismiss insertion */
				if (SLSearch(get_not_vaccinated_persons(v), &citizenID, compare_citizen) != NULL){
					/* If process is file parsing, then this record is considered inconsistent, so dismiss insertion */
					if (fileparse)
						return;
					/* If process is queries, then remove citizen from not_vaccinated persons skip list */
					else
						SLRemove(get_not_vaccinated_persons(v), &citizenID, compare_citizen);
				}

				date dateVaccinated = NULL;

				/* If user gave date, create date from given input */
				/* Else, vaccinated date is current date */
				if (str_date != NULL){
					if ( (dateVaccinated = create_date(str_date)) == NULL)
						return;					
				}
				else
					dateVaccinated = current_date();

				vaccinated_citizen = create_vaccinated(citizen, dateVaccinated);

				SLInsert(get_vaccinated_persons(v), vaccinated_citizen, get_vaccinated_key, compare_vaccinated);
				BloomInsert(get_filter(v), id);
			}
		}
		/* If citizen is not vaccinated, insert citizen to not_vaccinated_persons skip list*/
		else{
			/* Make sure citizen is not already in vaccinated_persons skip list */
			if ((vaccinated_citizen = SLSearch(get_vaccinated_persons(v), &citizenID, compare_vaccinated)) != NULL)
				return;
			else
				SLInsert(get_not_vaccinated_persons(v), citizen, get_citizenID, compare_citizen);
		}	
	}
}

void send_vaccineStatus(int citizenID, int conn_fd, int bufferSize){

	virus v = NULL;
	vaccinated vaccinated_citizen = NULL;

	/* For each of monitor's virus, send vaccineStatus */
	List head = NULL;
	for (int i = 0; i < HTSize(ds.viruses); i++){
		head = get_HTchain(ds.viruses, i);
		if(head != NULL){
			for (ListNode node = list_first(head); node != NULL; node = list_next(head, node)){
				v = list_node_item(head, node);
				char *virusName =  (char *)get_virusName(v);

				if ((vaccinated_citizen = SLSearch(get_vaccinated_persons(v), &citizenID, compare_vaccinated)) != NULL){
					char *dateVaccinated = get_date_as_str(get_vaccinated_date(vaccinated_citizen));
					char vaccineStatus[strlen(virusName) + strlen(dateVaccinated) + 17];
					snprintf(vaccineStatus, sizeof(vaccineStatus), "%s VACCINATED ON %s\n", virusName, dateVaccinated);
					free(dateVaccinated);
					send_data(conn_fd, bufferSize, vaccineStatus, 0);
				}
				else if (SLSearch(get_not_vaccinated_persons(v), &citizenID, compare_citizen) != NULL){
					char vaccineStatus[strlen(virusName) + 21];
					snprintf(vaccineStatus, sizeof(vaccineStatus), "%s NOT YET VACCINATED\n", virusName);
					send_data(conn_fd, bufferSize, vaccineStatus, 0);
				}
			}
		}
	}
	/* Inform the parent that monitor is ready to run queries */
	send_data(conn_fd, bufferSize, "end-vaccineStatus", 0);
}


void queries(int conn_fd, CyclicBuffer buffer, int bufferSize, int bloomSize){

	virus v = NULL;
	vaccinated vaccinated_citizen = NULL;

	while(true){

		char *line = receive_data(conn_fd, bufferSize);	

		char *query = strtok(line, " \n");

		if (strcmp(query, "/travelRequest") == 0){

			char *id = strtok(NULL, " \n");
			char *virusName = strtok(NULL, " \n");
			
			if ((v = HTSearch(ds.viruses, virusName, compare_virusName)) == NULL){ /* get virus */
				printf("Something went wrong\n");
				exit(1);
			}

			/* If citizen is in vaccinated_persons skip list */
			int citizenID = atoi(id);
			if ((vaccinated_citizen = SLSearch(get_vaccinated_persons(v), &citizenID, compare_vaccinated)) != NULL){			
				char *str_date = get_date_as_str(get_vaccinated_date(vaccinated_citizen));
				char response[strlen(str_date) + 5];
				snprintf(response, sizeof(response), "%s%s", "YES ", str_date);
				send_data(conn_fd, bufferSize, response, 0);

				free(str_date);
				continue;
			}
			else{
				send_data(conn_fd, bufferSize, "NO", 0);
			}
		}
		else if (strcmp(query, "/addVaccinationRecords") == 0){
			
			char *subdirPath = strtok(NULL, " \n");

			/* Open the country subdir */
			DIR *countryDir;
			if ((countryDir = opendir(subdirPath)) == NULL) {
				perror(RED "Error opening country's subdirectory" RESET);
				exit(EXIT_FAILURE);
			}
			// printf("subdirpath: %s\n", subdirPaths[i]);

			/* Traverse all the files in the country subdir */
			struct dirent *file;
			while ((file = readdir(countryDir)) != NULL) {
				
				/* Ignore the . and .. files */
				if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0)
					continue;

				/* Add the filename to parsed files, if it is not already parsed */
				char *filename = strdup(file->d_name);
				if (is_parsed(&ds, filename)){
					free(filename);
					continue;
				}
				list_insert_next(ds.parsed_files, NULL, filename);

				/* Create the path of file */
				char filePath[strlen(subdirPath) + strlen(filename) + 2];
				snprintf(filePath, sizeof(filePath), "%s/%s", subdirPath, filename);

				pthread_mutex_lock(&mtx); /* shared data area */

				/* If buffer is full, wait for signal nonfull */
				while (BuffFull(buffer)) {
					pthread_cond_wait(&nonfull, &mtx);
				}
				BuffInsert(buffer, strdup(filePath));

				pthread_mutex_unlock(&mtx);

				/* The buffer is not empty anymore (if it was) */
				pthread_cond_signal(&nonempty);
			}		

			/* Close the country subdir*/
			closedir(countryDir);

			/* Wait for buffer to empty, so all the files are parsed */
			while (!BuffEmpty(buffer)) { }
			send_bloomFilters(conn_fd, bufferSize, bloomSize);
		}
		else if (strcmp(query, "/searchVaccinationStatus") == 0){

			int citizenID = atoi(strtok(NULL, " \n"));

			/* Get citizen */
			citizenRecord c = NULL;
			if ((c = HTSearch(ds.citizens, &citizenID, compare_citizen)) == NULL)
				continue;

			char *citizen_info = get_citizen_info(c);
			send_data(conn_fd, bufferSize, citizen_info, 0);
			send_vaccineStatus(citizenID, conn_fd, bufferSize);

			free(citizen_info);
		}
		else if (strcmp(query, "/exit") == 0){
			/* Create the LogFiles dir with read/write/search permissions for owner, group and others */
    		mkdir("./LogFiles", S_IRWXU | S_IRWXG | S_IRWXO);

			/* Create the log_file.xxx for monitor with PID xxx and open it */
			char *filepath = concat_int_to_str("./LogFiles/log_file.", (int)getpid());
			FILE *logfile = NULL;
			if ((logfile = fopen(filepath, "w")) == NULL){
				perror(RED "Error opening Log file"  RESET);
				exit(EXIT_FAILURE);
			}

			/* Write to log_file.xxx the countries that monitor handles */
			List head = NULL;
			for (int i = 0; i < HTSize(ds.countries); i++){
				if((head = get_HTchain(ds.countries, i)) != NULL){
					for (ListNode node = list_first(head); node != NULL; node = list_next(head, node)){
						fprintf(logfile, "%s\n", (char *)get_country_name(list_node_item(head, node)));
					}
				}
			}

			/* Write to log_file.xxx the total number of requests */
			int accepted = ds.accepted_requests;
			int rejected = ds.rejected_requests;
			fprintf(logfile, "TOTAL TRAVEL REQUESTS %d\nACCEPTED %d\nREJECTED %d\n", accepted+rejected, accepted, rejected);

			fclose(logfile);
			free(line);
			break;
		}
		else{
			char *response = strtok(NULL, " \n");
			if (strcmp(response, "accepted") == 0)
				(ds.accepted_requests)++;
			else
				(ds.rejected_requests)++;	
		}
		free(line);
	}
}