/*  Grupo 039
 * Rafael Ribeiro   - 58193
 * Pedro Duque      - 52753
 * Francisco Santos - 59871
*/ 
#ifndef CONFIGURATION_H_GUARD
#define CONFIGURATION_H_GUARD

#define MAXLINESIZE 256

#include "main.h"

/* Função que abre o ficheiro de configuração passado com o caminho passado como
* argumento, e guarda os valores escritos dentro do ficheiro de configuração nos campos
* apropriados da estrutura data_container
*/
void read_config_information(char* file_path, struct data_container* data);

#endif