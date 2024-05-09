/*  Grupo 039
 * Rafael Ribeiro   - 58193
 * Pedro Duque      - 52753
 * Francisco Santos - 59871
*/ 
#ifndef CONFIGURATION_H_GUARD
#define CONFIGURATION_H_GUARD

#include <stdio.h>

#include "memory.h"

/* Função que abre o ficheiro de configuração inicial, recebendo um pointer um
* para o qual este é carregado, e o caminho do correspondente
*/
void openConfigFile(FILE* stream, char* filePath);

#endif