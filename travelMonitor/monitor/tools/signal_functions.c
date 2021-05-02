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

#include "signal_functions.h"

// /*  Shows whether a signal raised and awaits handling.
//     0 if no signal is pending, else 1. */
// static volatile sig_atomic_t sig_intquit_raised;
// static volatile sig_atomic_t sig_usr1_raised;

// /* Functions to handle signals */
// void handle_intquit(int signo) { sig_intquit_raised = signo; }
// void handle_usr1(int signo) { sig_usr1_raised = signo; }

// void set_signals(void){

// 	/* Signal sets to handle SIGINT/SIGQUIT and SIGUSR1 respectively */
// 	struct sigaction act_intquit, act_usr1;

//     /* Identify the action to be taken when the signal signo is received */
//     act_intquit.sa_handler = handle_intquit;
//     act_usr1.sa_handler = handle_usr1;

//     /* Create a full mask: the signals specified here will be
//        blocked during the execution of the sa_handler. */
//     sigfillset(&(act_intquit.sa_mask));
//     sigfillset(&(act_usr1.sa_mask));

//     /* Control specified signals */
//     sigaction(SIGINT, &act_intquit, NULL);
//     sigaction(SIGQUIT, &act_intquit, NULL);
//     sigaction(SIGUSR1, &act_usr1, NULL);

// }

// void check_signals(void){

//     if (sig_intquit_raised) {
//         fprintf(stderr, "SIGINT/SIGQUIT caught in main\n");
//     }

//     if (sig_usr1_raised) {
//         fprintf(stderr, "SIGUSR1 caught in main\n");
//     }

//     sig_intquit_raised = sig_usr1_raised = 0; /* reset signal flags */
// }