/*  Grupo 039
 * Rafael Ribeiro   - 58193
 * Pedro Duque      - 52753
 * Francisco Santos - 59871
*/ 
#ifndef LOG_H_GUARD
#define LOG_H_GUARD

#include <stdio.h>

#include "main.h"

/* Função que recebe a estrutura data_container, abre/cria o ficheiro de log com
* o nome da variável log_filename, e escreve uma operação ad com os valores dados
* para o ficheiro log, com o tempo na qual esta operação foi solicitada
*/
void log_ad(struct data_container* data, int patient_id, int doctor_id);

/* Função que recebe a estrutura data_container, abre/cria o ficheiro de log com
* o nome da variável log_filename, e escreve uma operação info com o valor dado
* para o ficheiro log, com o tempo na qual esta operação foi solicitada
*/
void log_info(struct data_container* data, int ad_id);

/* Função que recebe a estrutura data_container, abre/cria o ficheiro de log com
* o nome da variável log_filename, e escreve uma operação status para o ficheiro 
* log, com o tempo na qual esta operação foi solicitada
*/
void log_status(struct data_container* data);

/* Função que recebe a estrutura data_container, abre/cria o ficheiro de log com
* o nome da variável log_filename, e escreve uma operação help para o ficheiro 
* log, com o tempo na qual esta operação foi solicitada
*/
void log_help(struct data_container* data);

/* Função que recebe a estrutura data_container, abre/cria o ficheiro de log com
* o nome da variável log_filename, e escreve uma operação end para o ficheiro 
* log, com o tempo na qual esta operação foi solicitada
*/
void log_end(struct data_container* data);

/* Função que recebe o ficheiro de log e insere o tempo real no formato
* dia/mês/ano_hora:minuto:segundo.milisegundo
*/
void print_log_time(FILE* log_file);

#endif