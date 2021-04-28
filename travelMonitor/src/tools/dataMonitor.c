#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "dataMonitor.h"
#include "virus_bloom.h"

dataMonitor create_dataMonitor(pid_t pid){
    dataMonitor m = (dataMonitor)malloc(sizeof(struct dataMonitor_struct));

    m->pid = pid;
    m->countries = list_create(free);
    m->viruses = HTCreate(String, destroy_virus_bloom);

    return m;
}

void *get_dataMonitor_pid(void *m){
    dataMonitor nm = m;
    return &(nm->pid);    
}

List get_dataMonitor_countries(void *m){
    dataMonitor nm = m;
    return nm->countries;
}

HashTable get_dataMonitor_viruses(void *m){
    dataMonitor nm = m;
    return nm->viruses;
}

void add_dataMonitor_country(dataMonitor m, char *country){
    List head = m->countries;
    list_insert_next(head, list_last(head), country);
}

void print_dataMonitor(void *m){
    dataMonitor nm = m;
    printf("Monitor's PID: %d\n", nm->pid);
    printf("Countries of monitor:\n");

    List head = nm->countries;
    for (ListNode node = list_first(head); node != NULL; node = list_next(head, node))
		printf("%s\n", (char *)list_node_item(head, node));
    printf("\n");
}

int compare_dataMonitor(void *key, void *m){
    int k = *(int *)key;
    dataMonitor nm = m;

    if (nm->pid == k) return 0;
    else if (nm->pid > k) return -1;
    else return 1;
}

void destroy_dataMonitor(void *m){
    dataMonitor nm = m;
    list_destroy(nm->countries);
    HTDestroy(nm->viruses);
    free(nm);
}