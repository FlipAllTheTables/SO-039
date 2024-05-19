/*  Grupo 039
 * Rafael Ribeiro   - 58193
 * Pedro Duque      - 52753
 * Francisco Santos - 59871
*/ 

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "hospsignal.h"
#include "main.h"
#include "synchronization.h"

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

void set_alarm() {
    signal(SIGALRM, handle_alarm);
    struct itimerval val;
    val.it_value.tv_sec = data->alarm_time;
    val.it_value.tv_usec = 0;
    val.it_interval.tv_sec = data->alarm_time;
    val.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &val, 0);
}

void handle_alarm() {
    semaphore_lock(sems->results_mutex);
    for (int i = 0; i < data->max_ads; i++) {
        if (data->results[i].id != -1) {
            alarm_print_admission(&data->results[i]);
        }
    }
    semaphore_unlock(sems->results_mutex);
}

void alarm_print_admission(struct admission* ad) {
    printf("ad:%d status:%c start_time: %ld ", ad->id, ad->status, ad->create_time.tv_sec);
    if (ad->status != 'M') {
        printf("patient:%d patient_time: %ld ", ad->receiving_patient, ad->patient_time.tv_sec);
        if (ad->status != 'P') {
            printf("receptionist:%d receptionist_time: %ld ", ad->receiving_receptionist, ad->receptionist_time.tv_sec);
            if (ad->status != 'R') {
                printf("doctor:%d doctor_time: %ld ", ad->receiving_doctor, ad->doctor_time.tv_sec);
            }
        }
    }
    puts("");
}