/*  Grupo 039
 * Rafael Ribeiro   - 58193
 * Pedro Duque      - 52753
 * Francisco Santos - 59871
*/ 
#ifndef HOSPTIME_H_GUARD
#define HOSPTIME_H_GUARD

#include <time.h>

#include "memory.h"

/* Função que retorna uma estrutura timespec a partir da função clock_gettime
* com o clock real time
*/
struct timespec get_timespec();

#endif