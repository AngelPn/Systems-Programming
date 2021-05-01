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
				// printf("country_name: %s, subdirPath: %s\n\n", country_name, subdirPath);

				/* Open the country subdir */
				DIR *countryDir;
				if ((countryDir = opendir(subdirPath)) == NULL) {
					perror(RED "Error opening country's subdirectory" RESET);
					exit(EXIT_FAILURE);
				}

				/* Traverse all the files in the country subdri */
				struct dirent *file;
				while ((file = readdir(countryDir)) != NULL) {
					
					/* Ignore the . and .. files */
					if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0)
						continue;

					/* Add the filename to parsed files */
					char *filename = strdup(file->d_name);
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
				send_data(write_fd, bufferSize, (char *)get_virusName(v), 0);
				send_data(write_fd, bufferSize, get_array(get_filter(v)), bloomSize);
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


void vaccineStatus(void *item, int citizenID){

	virus v = item;

	vaccinated vaccinated_citizen = NULL;
	if ((vaccinated_citizen = SLSearch(get_vaccinated_persons(v), &citizenID, compare_vaccinated)) != NULL){
		printf("%s ", (char *)get_virusName(v));
		printf("YES ");
		print_vaccinated_date(vaccinated_citizen);
	}
	else if (SLSearch(get_not_vaccinated_persons(v), &citizenID, compare_citizen) != NULL){
		printf("%s ", (char *)get_virusName(v));
		printf("NO\n");
	}
	else return ;
}

void vaccineStatus_with_virusName(void *item, int citizenID){

	virus v = item;

	vaccinated vaccinated_citizen = NULL;
	if ((vaccinated_citizen = SLSearch(get_vaccinated_persons(v), &citizenID, compare_vaccinated)) != NULL){
		printf("VACCINATED ON ");
		print_vaccinated_date(vaccinated_citizen);
	}
	else printf("NOT VACCINATED\n");

}


void population_queries(char *args[5], dataStore *ds){

	virus v = NULL;
	country c = NULL;
	date date1 = NULL, date2 = NULL;

	/* If argument 0 is virusName */
	if ((v = HTSearch(ds->viruses, args[0], compare_virusName)) != NULL){

		/* If date1 exists, make sure date2 exists too */
		if ((date1 = create_date(args[1])) != NULL){
			if((date2 = create_date(args[2])) == NULL){
				printf(RED "\nERROR: date1 must come up with date2 in format: dd-mm-yy\n" RESET);
				free(date1);
				return;
			}
		}
		/* Take data from vaccinated_persons skip list */
		List head = get_bottom_level(get_vaccinated_persons(v));
		for (ListNode node = list_first(head); node != NULL; node = list_next(head, node)){

			vaccinated vaccinated_person = list_node_item(head, node);
			
			if (date_between(get_vaccinated_date(vaccinated_person), date1, date2)){
				if (strcmp(args[4], "/popStatusByAge") == 0)
					increase_popByAge_vaccinated(get_country(get_citizen(vaccinated_person)), get_vaccinated_citizen_age(vaccinated_person));
				else
					increase_vaccinated_persons(get_country(get_citizen(vaccinated_person)));
			}
		}
		/* Take data from not_vaccinated_persons skip list */
		head = get_bottom_level(get_not_vaccinated_persons(v));
		for (ListNode node = list_first(head); node != NULL; node = list_next(head, node)){

			citizenRecord person = list_node_item(head, node);
			
			if (strcmp(args[4], "/popStatusByAge") == 0)
				increase_popByAge_not_vaccinated(get_country(person), get_age(person));
			else
				increase_not_vaccinated_persons(get_country(person));
		}
		if (strcmp(args[4], "/popStatusByAge") == 0)
			HTVisit(ds->countries, popStatusByAge, 0);
		else
			HTVisit(ds->countries, populationStatus, 0);		
	}
	/* If argument 0 is country */	
	else if ((c = HTSearch(ds->countries, args[0], compare_countries)) != NULL){
		char *country_name = args[0];

		if ((v = HTSearch(ds->viruses, args[1], compare_virusName)) == NULL){
			printf(RED "\nERROR: virusName not in database\n" RESET);
			return;
		}
		/* If date1 exists, make sure date2 exists too */
		if ((date1 = create_date(args[2])) != NULL){
			if((date2 = create_date(args[3])) == NULL){
				printf(RED "\nERROR: date1 must come up with date2 in format: dd-mm-yy\n" RESET);
				free(date1);
				return;
			}
		}

		country curr_c = NULL;
		/* Take data from vaccinated_persons skip list */
		List head = get_bottom_level(get_vaccinated_persons(v));
		for (ListNode node = list_first(head); node != NULL; node = list_next(head, node)){

			vaccinated vaccinated_person = list_node_item(head, node);
			curr_c = get_country(get_citizen(vaccinated_person));

			if (!compare_countries(country_name, curr_c) && date_between(get_vaccinated_date(vaccinated_person), date1, date2)){
				if (strcmp(args[4], "/popStatusByAge") == 0)
					increase_popByAge_vaccinated(c, get_vaccinated_citizen_age(vaccinated_person));
				else
					increase_vaccinated_persons(c);
			}
		}
		/* Take data from not_vaccinated_persons skip list */
		head = get_bottom_level(get_not_vaccinated_persons(v));
		for (ListNode node = list_first(head); node != NULL; node = list_next(head, node)){

			citizenRecord person = list_node_item(head, node);
			curr_c = get_country(person);
			
			if(!compare_countries(country_name, curr_c)){
				if (strcmp(args[4], "/popStatusByAge") == 0)
					increase_popByAge_not_vaccinated(get_country(person), get_age(person));
				else
					increase_not_vaccinated_persons(get_country(person));				
			}
		}
		if (strcmp(args[4], "/popStatusByAge") == 0)
			popStatusByAge(c, 0);
		else populationStatus(c, 0);
	}
	else{
		printf(RED "\nERROR: virusName or country not in database\n" RESET
				YEL "Input format for this command: " RESET);

		if (strcmp(args[4], "/popStatusByAge") == 0)
			printf("/popStatusByAge [country] virusName [date1 date2]\n");
		else
			printf("/populationStatus [country] virusName [date1 date2]\n");
		return;
	}
	free(date1); free(date2);
}

void queries(dataStore *ds, int read_fd, int write_fd, int bufferSize){

	virus v = NULL;
	vaccinated vaccinated_citizen = NULL;
	// country c = NULL;

	while(true){
		char *line = receive_data(read_fd, bufferSize);
		printf("\nline in monitor: %s", line);

		char *query = strtok(line, " \n");

		if (strcmp(query, "/travelRequest") == 0){

			char *id = strtok(NULL, " \n");
			char *virusName = strtok(NULL, " \n");
			
			if ((v = HTSearch(ds->viruses, virusName, compare_virusName)) == NULL){ /* get virus */
				printf("Something went wrong\n");
				exit(1);
			}
			print_virus(v);

			printf("\nID: %s, virusName: %s\n", id, virusName);
			/* If citizen is in vaccinated_persons skip list */
			int citizenID = atoi(id);
			if ((vaccinated_citizen = SLSearch(get_vaccinated_persons(v), &citizenID, compare_vaccinated)) != NULL){
				printf("\nVACCINATED\n");
				char *str_date = get_date_as_str(get_vaccinated_date(vaccinated_citizen));
				char response[strlen(str_date) + 4];
				snprintf(response, sizeof(response), "%s %s", "YES ", str_date);
				send_data(write_fd, bufferSize, response, 0);
				free(str_date);
				continue;
			}
			else
				send_data(write_fd, bufferSize, "NO", 0);

		}
		else if (strcmp(query, "/travelStats") == 0){

		}
		else if (strcmp(query, "/addVaccinationRecords") == 0){

		}
		else if (strcmp(query, "/searchVaccinationStatus") == 0){

		}
		else if (strcmp(query, "/exit") == 0){

		}	

		free(line);
	}
}


void queries2(int bytes, dataStore *ds){

	/* Read input from stdin */
	char *line = NULL;
    size_t len = 0;
	virus v = NULL;
	bool broke = false;

	printf(GRN "\nEnter command:\n" RESET);

	while (getline(&line, &len, stdin) != -1){

		char *query = strtok(line, " \n");

		if (strcmp(query, "/vaccineStatusBloom") == 0){

			char *citizenID = strtok(NULL, " \n");
			char *virusName = strtok(NULL, " \n");

			if (citizenID != NULL && virusName != NULL){

				int id = atoi(citizenID);

				if (HTSearch(ds->citizens, &id, compare_citizen) == NULL){
					printf(RED "\nERROR: citizenID not in database\n" RESET);
					printf(GRN "\nEnter command:\n" RESET);
					continue;
				}

				v = HTSearch(ds->viruses, virusName, compare_virusName);
				if (v == NULL){
					printf(RED "\nERROR: virusName not in database\n" RESET);
				}
				else if (BloomSearch(get_filter(v), citizenID))
					printf("MAYBE\n");
				else printf("NOT VACCINATED\n");
			}
			else
				printf( RED "\nERROR: Invalid input\n" RESET
						YEL "Input format for this command: " RESET
						"/vaccineStatusBloom citizenID virusName\n");

		}
		else if (strcmp(query, "/vaccineStatus") == 0){
			
			char *citizenID = strtok(NULL, " \n");
			char *virusName = strtok(NULL, " \n");

			if (citizenID == NULL)
				printf( RED "\nERROR: Invalid input\n" RESET
						YEL "Input format for this command: " RESET
						"/vaccineStatus citizenID [virusName]\n");			
			else{
				int id = atoi(citizenID);

				if (HTSearch(ds->citizens, &id, compare_citizen) == NULL){
					printf(RED "\nERROR: citizenID not in database\n" RESET);
					printf(GRN "\nEnter command:\n" RESET);
					continue;
				}

				if (virusName != NULL){
					v = HTSearch(ds->viruses, virusName, compare_virusName);
					if (v == NULL)
						printf(RED "\nERROR: virusName not in database\n" RESET);
					else
						vaccineStatus_with_virusName(v, atoi(citizenID));
				}
				else
					HTVisit(ds->viruses, vaccineStatus, atoi(citizenID));				
			}
		}
		else if (strcmp(query, "/populationStatus") == 0 || strcmp(query, "/popStatusByAge") == 0){

			char *args[5];
			
			if ((args[0] = strtok(NULL, " \n")) == NULL){
				printf( RED "\nERROR: Invalid input\n" RESET
						YEL "Input format for this command: " RESET);
				if (strcmp(query, "/populationStatus") == 0)
					printf("/populationStatus");
				else
					printf("/popStatusByAge");
				printf(" [country] virusName [date1 date2]\n");
				printf(GRN "\nEnter command:\n" RESET);
				continue;	
			}
			
			for (int i = 1; i < 4; i++){
				args[i] = strtok(NULL, " \n");
			}
			args[4] = query;
			population_queries(args, ds);

		}
		else if (strcmp(query, "/insertCitizenRecord") == 0){

			char *args[8];
      		for (int i = 0; i < 8; i++){
				args[i] = strtok(NULL, " \n");

				if (args[i] == NULL && i < 7){
					printf( RED "\nERROR: Invalid input\n" RESET
							YEL "Input format for this command: " RESET
							"/insertCitizenRecord citizenID firstName lastName country age virusName YES/NO date\n"
							GRN "\nEnter command:\n" RESET);
					broke = true;
					break;
				}
			}
			if (broke){
				broke = false;
				continue;
			}
			insertCitizen(args, bytes, ds, false);

		}
		else if (strcmp(query, "/vaccinateNow") == 0){

			char *args[8];

      		for (int i = 0; i < 6; i++){
				args[i] = strtok(NULL, " \n");

				if (args[i] == NULL){
					printf( RED "\nERROR: Invalid input\n" RESET
							YEL "Input format for this command: " RESET
							"/vaccinateNow citizenID firstName lastName country age virusName\n"
							GRN "\nEnter command:\n" RESET);
					broke = true;
					break;
				}
			}
			if (broke){
				broke = false;
				continue;
			}
			args[6] = query;
			args[7] = NULL;
			insertCitizen(args, bytes, ds, false);

		}
		else if (strcmp(query, "/list-nonVaccinated-Persons") == 0){

			char *virusName = strtok(NULL, " \n");
			if (virusName != NULL){
				v = HTSearch(ds->viruses, virusName, compare_virusName);
				if (v == NULL)
					printf(RED "\nERROR: virusName not in database\n" RESET);
				else
					SLPrint_BottomLevel(get_not_vaccinated_persons(v), print_citizen);
			}
			else
				printf(RED "\nERROR: Invalid input\n" RESET
						YEL "Input format for this command: " RESET
						"/list-nonVaccinated-Persons virusName\n");

		}
		else if (strcmp(query, "/exit") == 0){
			printf("Exiting vaccineMonitor...\n");
			break;
		}
		else
			printf( RED "\nERROR: Invalid input\n" RESET
				YEL "Input format for every command:\n" RESET
				"/vaccineStatusBloom citizenID virusName\n"
				"/vaccineStatus citizenID [virusName]\n"
				"/populationStatus [country] virusName [date1 date2]\n"
				"/popStatusByAge [country] virusName [date1 date2]\n"
				"/insertCitizenRecord citizenID firstName lastName country age virusName YES/NO date\n"
				"/vaccinateNow citizenID firstName lastName country age virusName\n"
				"/list-nonVaccinated-Persons virusName\n"
				"/exit\n");
		
		printf(GRN "\nEnter command:\n" RESET);
	}
	free(line);
}