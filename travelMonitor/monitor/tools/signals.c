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

#include "signal.h"

/*  Shows whether a signal raised and awaits handling.
    0 if no signal is pending, else 1. */
volatile sig_atomic_t sig_int_raised;
volatile sig_atomic_t sig_usr1_raised;

/* Functions to catch signals */
void catch_int(int signo) { sig_int_raised = signo; }
void catch_usr1(int signo) { sig_usr1_raised = signo; }

void signals_config(void){
    struct sigaction act;
    
}

void signals_check(void){

}

void signals_block(void){

}

void signals_unblock(void){

}