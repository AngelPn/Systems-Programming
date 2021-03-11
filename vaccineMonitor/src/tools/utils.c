#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "utils.h"
#include "SkipList.h"
#include "date.h"
#include "virus.h"
#include "country.h"
#include "citizenRecord.h"

#define RED   "\033[1;31m"
#define GRN   "\033[1;32m"
#define YEL   "\033[1;33m"
#define BLU   "\033[1;34m"
#define MAG   "\033[1;35m"
#define CYN   "\033[1;36m"
#define WHT   "\033[1;37m"
#define RESET "\033[0m"

/* Does proper argument handling and stores variables from command prompt to vars bloomsize, filepath */
int argumentHandling(int argc, char **argv, int *bloomsize, char **filepath){

	for(int i = 1; i < argc; i = i + 2){
		
		if(strcmp(argv[i],"-c") == 0){
			if(i+1 < argc){
                char filecat[50] = "tests/";
                strcat(filecat, argv[i+1]);
                strcat(filecat, ".txt");
                *filepath = (char *)malloc(sizeof(char)*(strlen(filecat)+1));
				strcpy(*filepath, filecat);		
			}
			else{
				printf("ERROR: Invalid use of argument\n");				
				return 0;				
			}		
		}
		else if(strcmp(argv[i], "-b") == 0){
			if(i+1 < argc){
			*bloomsize = atoi(argv[i+1]);
				if(*bloomsize <= 0 ){
					printf("ERROR: Invalid argument after -b flag. Number must be positive integer.\n");
					return 0;
				}			
			}
			else{
				printf("ERROR: Invalid use of argument\n");				
				return 0;				
			}						
		} 			
		else{
			printf("ERROR: Invalid use of flags, the process will terminate\n");
			return 0;			
		}
		
	}
    return 1;
}


void insertCitizen(char *args[8], int kilobytes, dataStore *ds, bool fileparse);

/* Does file parsing and builds structs in dataStore */
void fileParse_and_buildStructs(char *filepath, int kilobytes, dataStore *ds){

	FILE *frecords;
    /* Open the file given from filepath and read it*/
    frecords = fopen(filepath, "r");

    if (frecords == NULL){
        printf("Error: fopen() failed\n");
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

		insertCitizen(args, kilobytes, ds, true);   
    }

	free(line); free(filepath);
    fclose(frecords);
}

void vaccineStatus(void *item, int citizenID){

	virus v = item;
	printf("%s ", (char *)get_virusName(v));

	vaccinated vaccinated_citizen = NULL;
	if ((vaccinated_citizen = SLSearch(get_vaccinated_persons(v), &citizenID, compare_vaccinated)) != NULL){
		printf("YES ");
		print_vaccinated_date(vaccinated_citizen);
	}
	else printf("NO\n");

}

void populationStatus(void *item, int key){

	country c = item;

	int vaccinated_persons = get_vaccinated_persons_num(c);
	if (vaccinated_persons){
		printf("population %d\n", get_population(c));
		printf("%s %d %.2f%%\n", (char *)get_country_name(c), vaccinated_persons, (double)vaccinated_persons/(double)get_population(c)*100);
		reset_vaccinated_persons(c);
	}
}

void popStatusByAge(void *item, int key){

	country c = item;

	int *vaccinated_persons = get_popByAge(c);
	if (vaccinated_persons != NULL){
		printf("\n%s\n", (char *)get_country_name(c));
		printf("0-20 %d %.2f%%\n", vaccinated_persons[0], (double)vaccinated_persons[0]/(double)get_population(c)*100);
		printf("20-40 %d %.2f%%\n", vaccinated_persons[1], (double)vaccinated_persons[1]/(double)get_population(c)*100);
		printf("40-60 %d %.2f%%\n", vaccinated_persons[2], (double)vaccinated_persons[2]/(double)get_population(c)*100);
		printf("60+ %d %.2f%%\n", vaccinated_persons[3], (double)vaccinated_persons[3]/(double)get_population(c)*100);
		reset_vaccinated_persons(c);
	}
	else{
		printf("\nNobody in country %s is vaccinated for this virus\n", (char *)get_country_name(c));
	}
}

void population_queries(char *args[5], dataStore *ds){

	virus v = NULL;
	country c = NULL;
	date start_date = NULL, end_date = NULL;
	char *virusName = NULL, *country_name = NULL;

	if ((v = HTSearch(ds->viruses, args[0], compare_virusName)) != NULL){

		virusName = args[0];

		if ((start_date = create_date(args[1])) != NULL){
			if((end_date = create_date(args[2])) == NULL){
				printf(RED "\nERROR: date1 must come up with date2\n" RESET);
				return;
			}
		}
		List head = get_bottom_level(get_vaccinated_persons(v));
		for (ListNode node = list_first(head); node != NULL; node = list_next(head, node)){

			vaccinated vaccinated_person = list_node_item(head, node);
			
			if (date_between(get_vaccinated_date(vaccinated_person), start_date, end_date)){
				if (strcmp(args[4], "/popStatusByAge") == 0)
					increase_popByAge(get_country(get_citizen(vaccinated_person)), get_vaccinated_citizen_age(vaccinated_person));
				else
					increase_vaccinated_persons(get_country(get_citizen(vaccinated_person)));
			}
		}
		if (strcmp(args[4], "/popStatusByAge") == 0)
			HTVisit(ds->countries, popStatusByAge, 0);
		else
			HTVisit(ds->countries, populationStatus, 0);		
	}
		
	if ((c = HTSearch(ds->countries, args[0], compare_countries)) != NULL){
		country_name = args[0];
	}

	

	/* Country is not given */
	if (args[3] == NULL){
		char *virusName = args[0];
		start_date = create_date(args[1]);
		end_date = create_date(args[2]);

		virus v = HTSearch(ds->viruses, virusName, compare_virusName);
		if (v == NULL){
			printf(RED "\nERROR: virusName not in database\n" RESET);
			return;
		}

		List head = get_bottom_level(get_vaccinated_persons(v));
		for (ListNode node = list_first(head); node != NULL; node = list_next(head, node)){

			vaccinated vaccinated_person = list_node_item(head, node);
			
			if (date_between(get_vaccinated_date(vaccinated_person), start_date, end_date)){
				if (strcmp(args[4], "/popStatusByAge") == 0)
					increase_popByAge(get_country(get_citizen(vaccinated_person)), get_vaccinated_citizen_age(vaccinated_person));
				else
					increase_vaccinated_persons(get_country(get_citizen(vaccinated_person)));
			}
		}
		if (strcmp(args[4], "/popStatusByAge") == 0)
			HTVisit(ds->countries, popStatusByAge, 0);
		else
			HTVisit(ds->countries, populationStatus, 0);
	}
	else{
		char *country_name = args[0];
		char *virusName = args[1];
		start_date = create_date(args[2]);
		end_date = create_date(args[3]);

		virus v = HTSearch(ds->viruses, virusName, compare_virusName);
		if (v == NULL){
			printf(RED "\nERROR: virusName not in database\n" RESET);
			return;
		}
		if (HTSearch(ds->countries, country_name, compare_countries) == NULL){
			printf(RED "\nERROR: country not in database\n" RESET);
			return;
		}
		
		country c = NULL; country curr_c = NULL;

		List head = get_bottom_level(get_vaccinated_persons(v));
		for (ListNode node = list_first(head); node != NULL; node = list_next(head, node)){

			vaccinated vaccinated_person = list_node_item(head, node);
			curr_c = get_country(get_citizen(vaccinated_person));

			if (!compare_countries(country_name, curr_c) && date_between(get_vaccinated_date(vaccinated_person), start_date, end_date)){
				c = curr_c;
				if (strcmp(args[4], "/popStatusByAge") == 0)
					increase_popByAge(c, get_vaccinated_citizen_age(vaccinated_person));
				else
					increase_vaccinated_persons(c);
			}
		}
		if (c == NULL){
			printf("Nobody in country %s is vaccinated for virus %s\n", country_name, virusName);
			return;
		}
		if (strcmp(args[4], "/popStatusByAge") == 0)
			popStatusByAge(c, 0);
		else populationStatus(c, 0);
	}

	free(start_date); free(end_date);
}

void insertCitizen(char *args[8], int kilobytes, dataStore *ds, bool fileparse){

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
		printf(RED "ERROR IN RECORD %s %s %s %s %s %s %s\n" RESET, id, firstname, lastname, country_name, age, check_vaccinated, str_date);
		return;
	}
	else{
		country c = NULL;
		citizenRecord citizen = NULL;
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
			citizen = create_citizen(citizenID, firstname, lastname, c, atoi(age));
			HTInsert(&(ds->citizens), citizen, get_citizenID);

			/* New citizen's record: increase the population of country */
			increase_population(c);
		}

		/* Check if virus is already in hash table of viruses */
		/* If not, insert virus (v) in hash table of viruses */
		if ((v = HTSearch(ds->viruses, virusName, compare_virusName)) == NULL){
			v = create_virus(virusName, kilobytes);
			HTInsert(&(ds->viruses), v, get_virusName);
		}

		/* If citizen is vaccinated, insert citizen to vaccinated_persons skip list and bloom filter */
		if (strcmp(check_vaccinated, "YES") == 0 || strcmp(check_vaccinated, "/vaccinateNow") == 0){

			vaccinated vaccinated_citizen = NULL;

			/* Make sure vaccinated citizen is not already in skip list */
			if ((vaccinated_citizen = SLSearch(get_vaccinated_persons(v), &citizenID, compare_vaccinated)) != NULL){
				printf(RED "ERROR: CITIZEN %d ALREADY VACCINATED ON " RESET, citizenID);
				print_vaccinated_date(vaccinated_citizen);
				return;
			}
			else{
				/* Search first if citizen is in not_vaccinated_persons to remove it or dismiss insertion */
				citizenRecord search_citizen = SLSearch(get_not_vaccinated_persons(v), &citizenID, compare_citizen);
				if (search_citizen != NULL){

					/* If process is file parsing, then this record is considered inconsistent */
					if (fileparse){
						printf(RED "ERROR: INCONSISTENT RECORD %s %s %s %s %s %s %s\n" RESET, id, firstname, lastname, country_name, age, check_vaccinated, str_date);
						return;
					}
					/* If process is queries, then remove citizen from not_vaccinated persons skip list */
					else
						SLRemove(get_not_vaccinated_persons(v), &citizenID, compare_citizen);
				}
				date dateVaccinated = NULL;

				/* If user gave date, create date from given input */
				/* Else, vaccinated date is current date */
				if (str_date != NULL)
					dateVaccinated = create_date(str_date);
				else
					dateVaccinated = current_date();

				if (dateVaccinated == NULL){
					printf(RED "ERROR: DATE IS NOT IN RIGHT FORMAT" RESET);
					return;
				}

				vaccinated_citizen = create_vaccinated(citizen, dateVaccinated);
				SLInsert(get_vaccinated_persons(v), vaccinated_citizen, get_vaccinated_key, compare_vaccinated);
				BloomInsert(get_filter(v), id);
			}
		}
		/* If citizen is not vaccinated, insert citizen to not_vaccinated_persons skip list*/
		else
			SLInsert(get_not_vaccinated_persons(v), citizen, get_citizenID, compare_citizen);
	}
}

void queries(int kilobytes, dataStore *ds){

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
				v = HTSearch(ds->viruses, virusName, compare_virusName);
				if (v == NULL){
					printf(RED "\nERROR: virusName not in database\n" RESET);
				}
				else if (BloomSearch(get_filter(v), citizenID))
					printf("MAYBE\n");
				else printf("NOT VACCINATED\n");
			}
			else
				printf( RED "\nERROR: No valid input\n" RESET
						YEL "Input format for this command: " RESET
						"/vaccineStatusBloom citizenID virusName\n");

		}
		else if (strcmp(query, "/vaccineStatus") == 0){
			
			char *citizenID = strtok(NULL, " \n");
			char *virusName = strtok(NULL, " \n");

			if (citizenID == NULL)
				printf( RED "\nERROR: No valid input\n" RESET
						YEL "Input format for this command: " RESET
						"/vaccineStatus citizenID [virusName]\n");				
			else{
				if (virusName != NULL){
					v = HTSearch(ds->viruses, virusName, compare_virusName);
					if (v == NULL){
						printf(RED "\nERROR: virusName not in database\n" RESET);
					}					
					vaccineStatus(v, atoi(citizenID));
				}
				else
					HTVisit(ds->viruses, vaccineStatus, atoi(citizenID));				
			}
		}
		else if (strcmp(query, "/populationStatus") == 0 || strcmp(query, "/popStatusByAge") == 0){

			char *args[5];
			
			if ((args[0] = strtok(NULL, " \n")) == NULL){
				printf( RED "\nERROR: No valid input\n" RESET
						YEL "Input format for this command: " RESET);
				if (strcmp(query, "/populationStatus") == 0)
					printf("/populationStatus");
				else
					printf("/popStatusByAge");
				printf(" [country] virusName [date1 date2]\n");
				return;		
			}
			
			for (int i = 1; i < 4; i++){
				args[i] = strtok(NULL, " \n");
			}
			args[4] = query;

			printf("args: %s %s %s %s %s\n", args[0], args[1], args[2], args[3], args[4]);
			population_queries(args, ds);
		}
		else if (strcmp(query, "/insertCitizenRecord") == 0){

			char *args[8];
      		for (int i = 0; i < 8; i++){
				args[i] = strtok(NULL, " \n");

				if (args[i] == NULL && i < 7){
					printf( RED "\nERROR: No valid input\n" RESET
							YEL "Input format for this command: " RESET
							"/insertCitizenRecord citizenID firstName lastName country age virusName YES/NO date\n"
							GRN "\nEnter command:\n" RESET);
					broke = true;
				}
			}
			if (broke){
				broke = false;
				continue;
			}
			insertCitizen(args, kilobytes, ds, false);

		}
		else if (strcmp(query, "/vaccinateNow") == 0){
			//printf("1\n");
			char *args[8];

      		for (int i = 0; i < 6; i++){
				args[i] = strtok(NULL, " \n");

				if (args[i] == NULL){
					printf( RED "\nERROR: No valid input\n" RESET
							YEL "Input format for this command: " RESET
							"/vaccinateNow citizenID firstName lastName country age virusName\n"
							GRN "\nEnter command:\n" RESET);
					broke = true;
				}
			}
			if (broke){
				broke = false;
				continue;
			}
			args[6] = query;
			args[7] = NULL;
			//printf("2 %s %s %s %s %s %s %s\n", args[0], args[1], args[2], args[3], args[4], args[5], args[6]);
			insertCitizen(args, kilobytes, ds, false);

		}
		else if (strcmp(query, "/list-nonVaccinated-Persons") == 0){

			char *virusName = strtok(NULL, " \n");
			if (virusName != NULL){
				v = HTSearch(ds->viruses, virusName, compare_virusName);
				SLPrint_BottomLevel(get_not_vaccinated_persons(v), print_citizen);
			}
			else
				printf(RED "\nERROR: No valid input\n" RESET
						YEL "Input format for this command: " RESET
						"/list-nonVaccinated-Persons virusName\n");

		}
		else if (strcmp(query, "/exit") == 0){
			printf("Exiting vaccineMonitor...\n");
			break;
		}
		else
			printf( RED "\nERROR: No valid input\n" RESET
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