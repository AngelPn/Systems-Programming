#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "monitor.h"

struct monitor_struct
{
    pid_t pid;          /* PID of monitor */
    int fd_index;       /* index of read_fd and write_fd arrays that keep the file descriptor */
    List countries;     /* list of the countries name that monitor handles */
    HashTable viruses;  /* hash table of viruses and bloom filters the monitor handles */
    int total_accepted; /* number of total accepted requests for monitor */
    int total_rejected; /* number of total rejected requests for monitor */
};

monitor create_monitor(pid_t pid, int fd_index){
    monitor m = (monitor)malloc(sizeof(struct monitor_struct));

    m->pid = pid;
    m->fd_index = fd_index;
    m->countries = list_create(free);
    m->viruses = HTCreate(String, destroy_virus_bloom);
    m->total_accepted = 0;
    m->total_rejected = 0;

    return m;
}

void *get_monitor_pid(void *m){
    monitor nm = m;
    return &(nm->pid);
}

int get_fd_index(monitor m){
    return m->fd_index;
}

int compare_monitor_country(void *country1, void *country2){
    return strcmp((char *)country1, (char *)country2);    
}

bool handles_country(monitor m, char *country){
    if (list_find(m->countries, country, compare_monitor_country) != NULL)
        return true;
    else return false;
}

List get_monitor_countries(void *m){
    monitor nm = m;
    return nm->countries;
}

HashTable get_monitor_viruses(void *m){
    monitor nm = m;
    return nm->viruses;
}

int get_total_accepted(monitor m){
    return m->total_accepted;
}

int get_total_rejected(monitor m){
    return m->total_rejected;
}

void change_pid(monitor m, pid_t new_pid){
    m->pid = new_pid;
}

void add_country(monitor m, char *country){
    List head = m->countries;
    list_insert_next(head, list_last(head), country);
}

void add_virus(monitor m, virus_bloom v){
    HTInsert(&(m->viruses), v, get_virus_bloomName);
}

void increase_accepted_requests(monitor m){
    (m->total_accepted)++;
}

void increase_rejected_requests(monitor m){
    (m->total_rejected)++;
}

void print_monitor(void *m){
    monitor nm = m;
    printf("Monitor's PID: %d\n", nm->pid);
    printf("Countries of monitor:\n");

    List head = nm->countries;
    for (ListNode node = list_first(head); node != NULL; node = list_next(head, node))
		printf("%s\n", (char *)list_node_item(head, node));
    printf("\n");
}

int compare_monitor(void *key, void *m){
    int k = *(int *)key;
    monitor nm = m;

    if (nm->pid == k) return 0;
    else if (nm->pid > k) return -1;
    else return 1;
}

void destroy_monitor(void *m){
    monitor nm = m;
    list_destroy(nm->countries);
    HTDestroy(nm->viruses);
    free(nm);
}