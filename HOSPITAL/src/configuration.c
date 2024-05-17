/*  Grupo 039
 * Rafael Ribeiro   - 58193
 * Pedro Duque      - 52753
 * Francisco Santos - 59871
*/ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "configuration.h"

void read_config_information(char* file_path, struct data_container* data) {
    // Valor máximo de caracteres por linha no ficheiro de configuração
    char line[MAXLINESIZE];

    // Ficheiro de configuração
    FILE* config_file;
    if ((config_file = fopen(file_path, "r")) == NULL) { // Verificar que ficheiro foi aberto corretamente
        puts("Erro em abertura de ficheiro de configuração.");
        exit(1);
    }

    if (fgets(line, MAXLINESIZE, config_file) != NULL) { // obter max_ads
        if ((data->max_ads = atoi(line)) < 1) {
            puts("max_ads tem de ser pelo menos 1.");
            exit(1);
        }
    } else {
        puts("Erro em obtenção de max_ads.");
        exit(1);
    }

    if (fgets(line, MAXLINESIZE, config_file) != NULL) { // obter buffers_size
        if ((data->buffers_size = atoi(line)) < 1) {
            puts("buffers_size tem de ser pelo menos 1.");
            exit(1);
        }
    } else {
        puts("Erro em obtenção de buffers_size.");
        exit(1);
    }

    if (fgets(line, MAXLINESIZE, config_file) != NULL) { // obter n_patients
        if ((data->n_patients = atoi(line)) < 1) {
            puts("n_patients tem de ser pelo menos 1.");
            exit(1);
        }
    } else {
        puts("Erro em obtenção de n_patients.");
        exit(1);
    }

    if (fgets(line, MAXLINESIZE, config_file) != NULL) { // obter n_receptionists
        if ((data->n_receptionists = atoi(line)) < 1) {
            puts("n_receptionist tem de ser pelo menos 1.");
            exit(1);
        }
    } else {
        puts("Erro em obtenção de n_receptionists.");
        exit(1);
    }

    if (fgets(line, MAXLINESIZE, config_file) != NULL) { // obter n_doctors
        if ((data->n_doctors = atoi(line)) < 1) {
            puts("n_doctors tem de ser pelo menos 1.");
            exit(1);
        }
    } else {
        puts("Erro em obtenção de n_doctors.");
        exit(1);
    }

    if (fgets(line, MAXLINESIZE, config_file) != NULL) { // obter log_filename
        int length = strlen(line);
        if (length > 0 && line[length-1] == '\n') { line[length-1] = '\0'; } // Substituir último caractere por '\0' se for '\n'
        strcpy(data->log_filename, line);
    } else {
        puts("Erro em obtenção de log_filename.");
        exit(1);
    }

    if (fgets(line, MAXLINESIZE, config_file) != NULL) { // obter statistics_filename
        int length = strlen(line);
        if (length > 0 && line[length-1] == '\n') { line[length-1] = '\0'; } // Substituir último caractere por '\0' se for '\n'
        strcpy(data->statistics_filename, line);
    } else {
        puts("Erro em obtenção de statistics_filename.");
        exit(1);
    }

    if (fgets(line, MAXLINESIZE, config_file) != NULL) { // obter alarm_time
        if ((data->alarm_time = atoi(line)) < 1) {
            puts("alarm_time tem de ser pelo menos 1.");
            exit(1);
        }
    } else {
        puts("Erro em obtenção de alarm_time.");
        exit(1);
    }

    // Verificar que max_ads não é superior ao número máximo de admissões pre-definido
    if (data->max_ads > MAX_RESULTS) {
        data->max_ads = MAX_RESULTS;
    }

    // Fechar ficheiro de configuração;
    fclose(config_file);
}