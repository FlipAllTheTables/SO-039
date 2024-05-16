/*  Grupo 039
 * Rafael Ribeiro   - 58193
 * Pedro Duque      - 52753
 * Francisco Santos - 59871
*/ 
#ifndef STATS_H_GUARD
#define STATS_H_GUARD

#include <time.h>

#include "main.h"
#include "memory.h"

/* Função que recebe a estrutura data_container e cria o ficheiro de estatísticas
* com o nome da variável statistics_filename, e escreve a estatísticas finais da
* executação de hOSpital
*/
void print_statistics(struct data_container* data);

/* Função que recebe o ficheiro de estatísticas e a estrutura data_container e escreve
* as estatísticas de cada paciente, receptionista e médico para o ficheiro de estatísticas
*/
void print_process_statistics(FILE* stats_file, struct data_container* data);

/* Função que recebe o ficheiro de estatísticas e a estrutura admission e escreve as
* estatísticas dessa admissão para o ficheiro de estatísticas
*/
void print_admission_statistics(FILE* stats_file, struct admission* ad);

/* Função que recebe o ficheiro de estatísticas e uma estutura timespec e escreve o tempo no
* formato dia/mês/ano_hora/minuto/segundo.milisegundos para o ficheiro de estatísticas
*/
void print_time_value(FILE* stats_file, struct timespec time);

#endif