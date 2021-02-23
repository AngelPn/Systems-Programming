#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/common_types.h"
#include "../include/ADTList.h"

// Function to print an integer
void printInt(void * n) {
	printf(" %d", *(int*)n); 
} 

int main(void){
    List list = list_create(NULL);

	int num = 8;
	list_insert_next(list, NULL, &num, sizeof(int));
	printf("Print the list: ");
	list_print(list, printInt);
	printf("\n");
	list_destroy(list);


	return 0;
}