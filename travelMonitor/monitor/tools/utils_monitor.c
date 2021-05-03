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

#include "utils_monitor.h"
#include "virus.h"
#include "country.h"
#include "ipc.h"
#include "BloomFilter.h"
#include "signal_functions.h"

#define RED   "\033[1;31m"
#define GRN   "\033[1;32m"
#define YEL   "\033[1;33m"
#define RESET "\033[0m"


void insertCitizen(char *args[8], int bytes, dataStore *ds, bool fileparse);

/* Does file parsing and builds structs in dataStore */
void fileParse_and_buildStructs(char *input_dir, int bytes, dataStore *ds){

	/* For each of monitor's assigned country subdirectory */
	List head = NULL;
	for (int i = 0; i < HTSize(ds->countries); i++){
		head = get_HTchain(ds->countries, i);
		if(head != NULL){
			for (ListNode node = list_first(head); node != NULL; node = list_next(head, node)){

				/* Get the country name and create the path of subdir */
				char *country_name = get_country_name(list_node_item(head, node));
				char subdirPath[strlen(input_dir) + strlen(country_name) + 2];
				snprintf(subdirPath, sizeof(subdirPath), "%s/%s", input_dir, country_name);
				// printf("country_name: %s, subdirPath: %s ->\n", country_name, subdirPath);

				/* Open the country subdir */
				DIR *countryDir;
				if ((countryDir = opendir(subdirPath)) == NULL) {
					perror(RED "Error opening country's subdirectory" RESET);
					exit(EXIT_FAILURE);
				}

				/* Traverse all the files in the country subdir */
				struct dirent *file;
				while ((file = readdir(countryDir)) != NULL) {
					
					/* Ignore the . and .. files */
					if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0)
						continue;

					/* Add the filename to parsed files, if it is not already parsed */
					char *filename = strdup(file->d_name);
					if (is_parsed(ds, filename)){
						free(filename);
						continue;
					}
					// printf("%s: parse\n", filename);
					list_insert_next(ds->parsed_files, NULL, filename);

					/* Create the path of file */
					char filePath[strlen(subdirPath) + strlen(filename) + 2];
					snprintf(filePath, sizeof(filePath), "%s/%s", subdirPath, filename);

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
						for (int i = 1; i < 8; ++i)
							args[i] = strtok(NULL, " \n");

						insertCitizen(args, bytes, ds, true);   
					}

					free(line);
					fclose(frecords);
				}
			}
		}		
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


void send_bloomFilters(dataStore *ds, int write_fd, int bufferSize, int bloomSize){

	virus v = NULL;

	/* For each of monitor's virus, send virus name and bloom filter of virus */
	List head = NULL;
	for (int i = 0; i < HTSize(ds->viruses); i++){
		head = get_HTchain(ds->viruses, i);
		if(head != NULL){
			for (ListNode node = list_first(head); node != NULL; node = list_next(head, node)){
				v = list_node_item(head, node);
				char *virusName =  (char *)get_virusName(v);
				send_data(write_fd, bufferSize, virusName, 0);
				// send_data(write_fd, bufferSize, get_array(get_filter(v)), bloomSize);
				send_bloom_filter(get_filter(v), write_fd, bufferSize);
			}
		}
	}
	/* Inform the parent that monitor is ready to run queries */
	send_data(write_fd, bufferSize, "ready", 0);
}


void insertCitizen(char *args[8], int bytes, dataStore *ds, bool fileparse){

	/* Get data from arguments with right order */
	char *id = args[0];
	char *firstname = args[1];
	char *lastname = args[2];
	char *country_name = args[3];
	char *age = args[4];
	char *virusName = args[5];
	char *check_vaccinated = args[6];
	char *str_date = args[7];
	
	if (strcmp(check_vaccinated, "NO") == 0 && str_date != NULL){
		printf(RED "ERROR IN RECORD %s %s %s %s %s %s %s %s\n" RESET, id, firstname, lastname, country_name, age, virusName, check_vaccinated, str_date);
		return;
	}
	else{
		country c = NULL;
		citizenRecord citizen = NULL;
		vaccinated vaccinated_citizen = NULL;
		virus v = NULL;
		
		/* Check if country is already in hash table of countries */
		/* If not, insert country (c) in hash table of countries */
		if ((c = HTSearch(ds->countries, country_name, compare_countries)) == NULL ){
			c = create_country(country_name);
			HTInsert(&(ds->countries), c, get_country_name);
		}

		/* Check if citizen is already in hash table of citizens */
		/* If not, insert citizen in hash table of citizens */
		int citizenID = atoi(id);
		if ((citizen = HTSearch(ds->citizens, &citizenID, compare_citizen)) == NULL){
			if ((citizen = create_citizen(citizenID, firstname, lastname, c, atoi(age))) == NULL){
				printf(RED "ERROR: Given citizen's age is not between 0 and 120\n" RESET);
				return;
			}
			else
				HTInsert(&(ds->citizens), citizen, get_citizenID);
		}
		/* If citizen is already in hash tbale of citizens, cross-check given data */
		else{
			if (!cross_check(citizen, firstname, lastname, c, atoi(age))){
				printf(RED "ERROR: Given citizen's data do not match with data in database\n" RESET
						"Data in database:\n");
				print_citizen(citizen);
				return;
			}
		}

		/* Check if virus is already in hash table of viruses */
		/* If not, insert virus (v) in hash table of viruses */
		if ((v = HTSearch(ds->viruses, virusName, compare_virusName)) == NULL){
			v = create_virus(virusName, bytes);
			HTInsert(&(ds->viruses), v, get_virusName);
		}

		/* If citizen is vaccinated, insert citizen to vaccinated_persons skip list and bloom filter */
		if (strcmp(check_vaccinated, "YES") == 0 || strcmp(check_vaccinated, "/vaccinateNow") == 0){

			/* Make sure vaccinated citizen is not already in skip list */
			if ((vaccinated_citizen = SLSearch(get_vaccinated_persons(v), &citizenID, compare_vaccinated)) != NULL){
				printf(RED "ERROR: CITIZEN %d ALREADY VACCINATED ON " RESET, citizenID);
				print_vaccinated_date(vaccinated_citizen);
				return;
			}
			else{
				/* Search first if citizen is in not_vaccinated_persons to remove it or dismiss insertion */
				if (SLSearch(get_not_vaccinated_persons(v), &citizenID, compare_citizen) != NULL){
					/* If process is file parsing, then this record is considered inconsistent, so dismiss insertion */
					if (fileparse){
						printf(RED "ERROR: INCONSISTENT RECORD %s %s %s %s %s %s %s %s\n" RESET, id, firstname, lastname, country_name, age, virusName, check_vaccinated, str_date);
						return;
					}
					/* If process is queries, then remove citizen from not_vaccinated persons skip list */
					else
						SLRemove(get_not_vaccinated_persons(v), &citizenID, compare_citizen);
				}

				date dateVaccinated = NULL;

				/* If user gave date, create date from given input */
				/* Else, vaccinated date is current date */
				if (str_date != NULL){
					if ( (dateVaccinated = create_date(str_date)) == NULL){
						printf(RED "ERROR: DATE IS NOT IN RIGHT FORMAT\n" RESET);
						return;
					}					
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
			if ((vaccinated_citizen = SLSearch(get_vaccinated_persons(v), &citizenID, compare_vaccinated)) != NULL){
				printf(RED "ERROR: CITIZEN %d ALREADY VACCINATED ON " RESET, citizenID);
				print_vaccinated_date(vaccinated_citizen);
				return;
			}
			else
				SLInsert(get_not_vaccinated_persons(v), citizen, get_citizenID, compare_citizen);
		}	
	}
}



/*  Shows whether a signal raised and awaits handling.
    0 if no signal is pending, else 1. */
static volatile sig_atomic_t sig_intquit_raised;
static volatile sig_atomic_t sig_usr1_raised;

/* Functions to handle signals */
void handle_intquit(int signo) { sig_intquit_raised = signo; }
void handle_usr1(int signo) { sig_usr1_raised = signo; }



void queries(dataStore *ds, char *input_dir, int read_fd, int write_fd, int bufferSize, int bloomSize){

	/* Signal sets to handle SIGINT/SIGQUIT and SIGUSR1 respectively */
	struct sigaction act_intquit, act_usr1;

    /* Identify the action to be taken when the signal signo is received */
    // act_intquit.sa_handler = handle_intquit;
    act_usr1.sa_handler = handle_usr1;

    /* Create a full mask: the signals specified here will be
       blocked during the execution of the sa_handler. */
    sigfillset(&(act_intquit.sa_mask));
    sigfillset(&(act_usr1.sa_mask));

    /* Control specified signals */
    // sigaction(SIGINT, &act_intquit, NULL);
    // sigaction(SIGQUIT, &act_intquit, NULL);
    sigaction(SIGUSR1, &act_usr1, NULL);

	virus v = NULL;
	vaccinated vaccinated_citizen = NULL;
	// country c = NULL;

	while(true){
		printf("in while loop\n");

		char *line = receive_data(read_fd, bufferSize);
		
		char *query = strtok(line, " \n");
		printf("NOT blocking, query: %s\n", query);
		if (sig_usr1_raised) {
			fprintf(stderr, "SIGUSR1 caught in main\n");

			/* Parse the added files */
			fileParse_and_buildStructs(input_dir, bloomSize, ds);
			// printf("PARSE DONE\n");
			send_bloomFilters(ds, write_fd, bufferSize, bloomSize);
			// printf("SENT BLOOM\n");

			/* Inform the parent that we've read stuff by sending a SIGUSR2 */
			// kill(getppid(), SIGUSR2);
			
			sig_usr1_raised = 0; /* reset value */
			continue;
		}

		if (strcmp(query, "/travelRequest") == 0){

			char *id = strtok(NULL, " \n");
			char *virusName = strtok(NULL, " \n");
			
			if ((v = HTSearch(ds->viruses, virusName, compare_virusName)) == NULL){ /* get virus */
				printf("Something went wrong\n");
				exit(1);
			}
			// printf("\n--------------MONITOR----------\n");
    		// print_bl(get_filter(v));
			if (!(BloomSearch(get_filter(v), id))){
				printf("NOT IN BLOOM FILTER");
			}
			/* If citizen is in vaccinated_persons skip list */
			int citizenID = atoi(id);
			if ((vaccinated_citizen = SLSearch(get_vaccinated_persons(v), &citizenID, compare_vaccinated)) != NULL){			
				char *str_date = get_date_as_str(get_vaccinated_date(vaccinated_citizen));
				char response[strlen(str_date) + 5];
				snprintf(response, sizeof(response), "%s%s", "YES ", str_date);
				// printf("response: %s\n", response);
				send_data(write_fd, bufferSize, response, 0);
				free(str_date);
				continue;
			}
			else
				send_data(write_fd, bufferSize, "NO", 0);

		}
		else if (strcmp(query, "/searchVaccinationStatus") == 0){

			int citizenID = atoi(strtok(NULL, " \n"));

			/* Get citizen */
			citizenRecord citizen = NULL;
			if ((c = HTSearch(ds->citizens, citizenID, compare_citizen)) == NULL)
				continue;
			

		}
		else if (strcmp(query, "/exit") == 0){

		}
		else if (query == NULL){/* Check for a possible signal */
			printf("here\n");
		}

		free(line);
	}
}