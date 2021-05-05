#ifndef QUERIES_H
#define QUERIES_H

void run_queries(HashTable *monitors, int bufferSize, int bloomSize, pid_t *monitors_pids, int *read_fd, int *write_fd, int numActiveMonitors);

#endif