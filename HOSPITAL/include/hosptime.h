/*  Grupo 039
 * Rafael Ribeiro   - 58193
 * Pedro Duque      - 52753
 * Francisco Santos - 59871
*/ 
#ifndef HOSPTIME_H_GUARD
#define HOSPTIME_H_GUARD

#include "memory.h"

/* Função que regista o tempo de criação no campo correspondente da admissão
* recebida como argumento
*/
void register_create_time(struct admission* ad);

/* Função que regista o tempo em que o paciente recebeu a admissão no campo
* correspondente da admissão recebida como argumento
*/
void register_patient_time(struct admission* ad);

/* Função que regista o tempo em que o rececionista recebeu a admissão no campo
* correspondente da admissão recebida como argumento
*/
void register_receptionist_time(struct admission* ad);

/* Função que regista o tempo em que o médico recebeu a admissão no campo
* correspondente da admissão recebida como argumento
*/
void register_doctor_time(struct admission* ad);

#endif