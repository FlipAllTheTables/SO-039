#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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