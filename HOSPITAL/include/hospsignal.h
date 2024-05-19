/*  Grupo 039
 * Rafael Ribeiro   - 58193
 * Pedro Duque      - 52753
 * Francisco Santos - 59871
*/ 
#ifndef HOSPSIGNAL_H_GUARD
#define HOSPSIGNAL_H_GUARD

#include "memory.h"

/* Função que define o comportamento do processo para executar a função signal_ctrl_c
* quando é enviado o sinal SIGINT
*/
void set_sigint();

/* Função que define o comportamento do processo para ignorar o sinal SIGINT quando este 
* é enviado. Esta função deve ser usada para que os filhos do processo pai ignorem este
* sinal, sendo que apenas o processo pai deve tratar do sinal SIGINT
*/
void signal_ignore();

/* Função que é executada quando é enviado o sinal SIGINT, que por si executa a função
* end_execution, e termina executação de hOSpital. Apenas o processo pai tem a capacidade
* de terminar a executação
*/
void ctrl_c();

/* Função que cria um temporizador que expira no tempo definido pela variável alarm_time
* dentro da estrutura data_container, e define como é tratado o sinal SIGALRM, chamando a
* função handle_alarm
*/
void set_alarm();

/* Função que imprime informação sobre todas as admissões disponíveis, executada apenas
* quando é recebido o sinal SIGALRM
*/
void handle_alarm();

/* Função que imprime a admissão fornecida no formato:
* ad status start_time receiving_patient patient_time receiving_recptionist
* receptionist_time receiving_doctor doctor_time
* Sendo que os valores de tempo são escritos no formato "raw", em segundos
*/
void alarm_print_admission(struct admission* ad);

#endif