#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "monitor.h"

struct monitor_struct
{
    pid_t pid;
    List countries;
};

monitor create_monitor(pid_t pid){
    monitor m = (monitor)malloc(sizeof(struct monitor_struct));

    m->pid = pid;
    m->countries = list_create(free);

    return m;
}

void *get_monitor_pid(void *m){
    monitor nm = m;
    return &(nm->pid);
}

List get_monitor_countries(void *m){
    monitor nm = m;
    return nm->countries;
}

void add_country(monitor m, char *country){
    List head = m->countries;
    list_insert_next(head, list_last(head), country);
}

void print_monitor(void *m){
    monitor nm = m;
    printf("Monitor's PID: %d\n", nm->pid);
    printf("Countries of monitor:\n");

    List head = nm->countries;
    for (ListNode node = list_first(head); node != NULL; node = list_next(head, node))
		printf("%s\n", (char *)list_node_item(head, node));
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
    free(nm);
}