#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "utils.h"
#include "../../mylib/SkipList/SkipList.h"
#include "../../mylib/Date/date.h"
#include "virus.h"
#include "citizenRecord.h"

/* Does proper error handling and stores variables from command prompt */
int argumentHandling(int argc, char **argv, int *bloomsize, char **filepath){

	for(int i = 1; i < argc; i = i + 2){
		
		if(strcmp(argv[i],"-c") == 0){
			if(i+1 < argc){
                char filecat[50] = "test/";
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

void *get_country(void *country){
	return country;
}

int compare_countries(void *a, void *b){
	char *country_a = a;
	char *country_b = b;
	return strcmp(country_a, country_b);
}

void insertCitizen(char *args[8], int kilobytes, HashTable *citizens, HashTable *viruses, HashTable *countries, bool fileparse);

void fileParse_and_buildStructs(char *filepath, int kilobytes, HashTable *citizens, HashTable *viruses, HashTable *countries){

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

		insertCitizen(args, kilobytes, citizens, viruses, countries, true);   
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

void insertCitizen(char *args[8], int kilobytes, HashTable *citizens, HashTable *viruses, HashTable *countries, bool fileparse){

	citizenRecord citizen = NULL;
	virus v = NULL;
	char *country = NULL;

	/* Get citizen's information and build citizen's struct (citizenRecord) */
	char *id = args[0];
	char *firstname = args[1];
	char *lastname = args[2];

	/* Check if country is already in database of countries (HashTable countries) */
	/* If not, insert country in database of countries */
	char *country_name = args[3];

	if ((country = HTSearch(*countries, country_name, compare_countries)) == NULL ){
		country = (char *)malloc(sizeof(char)*(strlen(country_name)+1));
		strcpy(country, country_name);
		HTInsert(countries, country, get_country);
	}

	char *age = args[4];

	/* Check if citizen is already in database of citizens (HashTable citizens) */
	/* If not, insert citizen in database of citizens */
	int citizenID = atoi(id);
	if ((citizen = HTSearch(*citizens, &citizenID, compare_citizen)) == NULL){
		citizen = create_citizen(citizenID, firstname, lastname, country, atoi(age));
		HTInsert(citizens, citizen, get_citizenID);
	}

	/* Get the name of virus and check if it is already in database of viruses (HashTable viruses) */
	/* If not, insert virus (v) in database of viruses */
	char *virusName = args[5];
	if ((v = HTSearch(*viruses, virusName, compare_virusName)) == NULL){
		v = create_virus(virusName, kilobytes);
		HTInsert(viruses, v, get_virusName);
	}

	/* Check if citizen is vaccinated to virus or not */
	char *check_vaccinated = args[6];
	char *str_date = args[7];

	/* If citizen is vaccinated, get the date and insert this information to vaccinated_persons skip list*/
	if (strcmp(check_vaccinated, "YES") == 0 || strcmp(check_vaccinated, "/vaccinateNow") == 0){

		vaccinated vaccinated_citizen = NULL;

		if ((vaccinated_citizen = SLSearch(get_vaccinated_persons(v), &citizenID, compare_vaccinated)) != NULL){
			printf("ERROR: CITIZEN %d ALREADY VACCINATED ON ", citizenID);
			print_vaccinated_date(vaccinated_citizen);
		}
		else{
			/* Search first if citizen is in not_vaccinated_persons to remove it or dismiss insertion */
			citizenRecord search_citizen = SLSearch(get_not_vaccinated_persons(v), &citizenID, compare_citizen);
			if (search_citizen != NULL){

				/* If process is file parsing, then this record is considered inconsistent */
				if (fileparse){
					printf("ERROR IN RECORD %s %s %s %s %s %s %s\n", id, firstname, lastname, country_name, age, check_vaccinated, str_date);
					return;
				}
				/* If process is queries, then remove citizen from not_vaccinated persons skip list */
				else{
					SLRemove(get_not_vaccinated_persons(v), &citizenID, compare_citizen);
				}
			}
			date dateVaccinated = NULL;

			/* If user gave date, create date from given input */
			/* Else, vaccinated date is current date */
			if (str_date != NULL)
				dateVaccinated = create_date(str_date);
			else
				dateVaccinated = current_date();
			print_date(dateVaccinated);
			vaccinated_citizen = create_vaccinated(citizen, dateVaccinated);
			print_vaccinated(vaccinated_citizen);
			SLInsert(get_vaccinated_persons(v), vaccinated_citizen, get_vaccinated_key, compare_vaccinated, print_vaccinated);
			BloomInsert(get_filter(v), id);
		}

	}
	/* If citizen is not vaccinated, insert this information to not_vaccinated_persons skip list*/
	else{
		/* If there is date, then print ERROR */
		if (str_date != NULL){
			printf("ERROR IN RECORD %s %s %s %s %s %s %s\n", id, firstname, lastname, country_name, age, check_vaccinated, str_date);
			return;
		}
		SLInsert(get_not_vaccinated_persons(v), citizen, get_citizenID, compare_citizen, print_citizen);                
	}

}

void queries(int kilobytes, HashTable *citizens, HashTable *viruses, HashTable *countries){

	/* Read input from stdin */
	char *line = NULL;
    size_t len = 0;
	//citizenRecord citizen = NULL;
	//vaccinated vaccinated_citizen = NULL;
	virus v = NULL;

	while (getline(&line, &len, stdin) != -1){

		char *query = strtok(line, " \n");

		if (strcmp(query, "/vaccineStatusBloom") == 0){

			char *citizenID = strtok(NULL, " \n");
			char *virusName = strtok(NULL, " \n");

			if (virusName != NULL){
				v = HTSearch(*viruses, virusName, compare_virusName);
				if (BloomSearch(get_filter(v), citizenID))
					printf("MAYBE\n");
				else printf("NOT VACCINATED\n");
			}
			else
				printf("Error in input\n");

		}
		else if (strcmp(query, "/vaccineStatus") == 0){
			
			int citizenID = atoi(strtok(NULL, " \n"));
			char *virusName = strtok(NULL, " \n");

			if (virusName != NULL){
				v = HTSearch(*viruses, virusName, compare_virusName);
				vaccineStatus(v, citizenID);
			}
			else
				HTVisit(*viruses, vaccineStatus, citizenID);

		}
		else if (strcmp(query, "/populationStatus") == 0){

		}
		else if (strcmp(query, "/popStatusByAge") == 0){

		}
		else if (strcmp(query, "/insertCitizenRecord") == 0){

			char *args[8];
      		for (int i = 0; i < 8; i++){
				args[i] = strtok(NULL, " \n");

				if (args[i] == NULL && i < 7){
					printf("Error: No valid input. Enter new command:\n");
					continue;
				}
			}
			insertCitizen(args, kilobytes, citizens, viruses, countries, false);

		}
		else if (strcmp(query, "/vaccinateNow") == 0){
			//printf("1\n");
			char *args[8];

      		for (int i = 0; i < 6; i++){
				args[i] = strtok(NULL, " \n");

				if (args[i] == NULL){
					printf("Error: No valid input. Enter new command:\n");
					continue;
				}
			}
			args[6] = query;
			args[7] = NULL;
			//printf("2 %s %s %s %s %s %s %s\n", args[0], args[1], args[2], args[3], args[4], args[5], args[6]);
			insertCitizen(args, kilobytes, citizens, viruses, countries, false);

		}
		else if (strcmp(query, "/list-nonVaccinated-Persons") == 0){

			char *virusName = strtok(NULL, " \n");
			if (virusName != NULL){
				v = HTSearch(*viruses, virusName, compare_virusName);
				SLPrint(get_not_vaccinated_persons(v), print_citizen);
			}
			else printf("Error: No valid input. Enter new command:\n");

		}
		else if (strcmp(query, "/exit") == 0){
			printf("exiting\n");
			break;
		}
		else printf("Error in command\n");
	}
	free(line);
}