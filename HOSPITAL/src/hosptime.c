/*  Grupo 039
 * Rafael Ribeiro   - 58193
 * Pedro Duque      - 52753
 * Francisco Santos - 59871
*/ 

#include <stdio.h>
#include <stdlib.h>

#include "hosptime.h"
#include "memory.h"

struct timespec get_timespec() {
    // Registo de tempo
    struct timespec time;

    // Obter valor real time 
    if( clock_gettime(CLOCK_REALTIME, &time) == -1 ) {
        puts("Erro glock_gettime.");
        exit(1);
    }
    return time;
}