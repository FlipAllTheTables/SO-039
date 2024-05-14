#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "hosptime.h"
#include "memory.h"

void register_create_time(struct admission* ad) {
    // Registo de tempo
    struct timespec time;

    // Obter valor real time 
    if( clock_gettime(CLOCK_REALTIME, &time) == -1 ) {
        puts("Erro glock_gettime.");
        exit(1);
    }
    // Registar tempo no campo apropriado da admissão
    ad->create_time = time;
}

void register_patient_time(struct admission* ad) {
    // Registo de tempo
    struct timespec time;

    // Obter valor real time 
    if( clock_gettime(CLOCK_REALTIME, &time) == -1 ) {
        puts("Erro glock_gettime.");
        exit(1);
    }
    // Registar tempo no campo apropriado da admissão
    ad->patient_time = time;
}

void register_receptionist_time(struct admission* ad) {
    // Registo de tempo
    struct timespec time;

    // Obter valor real time 
    if( clock_gettime(CLOCK_REALTIME, &time) == -1 ) {
        puts("Erro glock_gettime.");
        exit(1);
    }
    // Registar tempo no campo apropriado da admissão
    ad->receptionist_time = time;
}

void register_doctor_time(struct admission* ad) {
    // Registo de tempo
    struct timespec time;

    // Obter valor real time 
    if( clock_gettime(CLOCK_REALTIME, &time) == -1 ) {
        puts("Erro glock_gettime.");
        exit(1);
    }
    // Registar tempo no campo apropriado da admissão
    ad->doctor_time = time;
}