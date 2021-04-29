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
};

monitor create_monitor(pid_t pid, int fd_index){
    monitor m = (monitor)malloc(sizeof(struct monitor_struct));

    m->pid = pid;
    m->fd_index = fd_index;
    m->countries = list_create(free);
    m->viruses = HTCreate(String, destroy_virus_bloom);

    return m;
}

void *get_monitor_pid(void *m){
    monitor nm = m;
    return &(nm->pid);
}

int get_fd_index(monitor m){
    return m->fd_index;
}

bool handles_country(monitor m, char *country){
    if (list_find(m->countries, country, strcmp) != NULL)
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

void add_country(monitor m, char *country){
    List head = m->countries;
    list_insert_next(head, list_last(head), country);
}

void add_virus(monitor m, virus_bloom v){
    HTInsert(&(m->viruses), v, get_virus_bloomName);
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