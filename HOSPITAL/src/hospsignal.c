/*  Grupo 039
 * Rafael Ribeiro   - 58193
 * Pedro Duque      - 52753
 * Francisco Santos - 59871
*/ 

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "hospsignal.h"
#include "main.h"

// Variáveis globais de main.c
extern struct data_container* data;
extern struct communication* comm;
extern struct semaphores* sems;

void set_sigint() {
    signal(SIGINT, ctrl_c);
}

void signal_ignore() {
    signal(SIGINT, SIG_IGN);
}

void ctrl_c() {
    end_execution(data, comm, sems);
    exit(0);
}