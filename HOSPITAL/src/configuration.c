/*  Grupo 039
 * Rafael Ribeiro   - 58193
 * Pedro Duque      - 52753
 * Francisco Santos - 59871
*/ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "configuration.h"
#include "main.h"

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
        data->max_ads = atoi(line);
    } else {
        puts("Erro em obtenção de max_ads.");
        exit(1);
    }

    if (fgets(line, MAXLINESIZE, config_file) != NULL) { // obter buffers_size
        data->buffers_size = atoi(line);
    } else {
        puts("Erro em obtenção de buffers_size.");
        exit(1);
    }

    if (fgets(line, MAXLINESIZE, config_file) != NULL) { // obter n_patients
        data->n_patients = atoi(line);
    } else {
        puts("Erro em obtenção de n_patients.");
        exit(1);
    }

    if (fgets(line, MAXLINESIZE, config_file) != NULL) { // obter n_receptionists
        data->n_receptionists = atoi(line);
    } else {
        puts("Erro em obtenção de n_receptionists.");
        exit(1);
    }

    if (fgets(line, MAXLINESIZE, config_file) != NULL) { // obter n_doctors
        data->n_doctors = atoi(line);
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
        data->alarm_time = atoi(line);
    } else {
        puts("Erro em obtenção de alarm_time.");
        exit(1);
    }

    // Verificar que max_ads não é superior ao número máximo de admissões pre-definido
    if (data->max_ads > MAX_RESULTS) {
        data->max_ads = MAX_RESULTS;
    }

    printf("Configuração da estrutura data_container feita!\n");
    printf("max_ads: %d\nbuffers_size: %d\nn_patients: %d\n", data->max_ads, data->buffers_size, data->n_patients);
    printf("n_receptionists: %d\nn_doctors: %d\nlog_filename: %s\n", data->n_receptionists, data->n_doctors, data->log_filename);
    printf("statistics_filename: %s\nalarm_time: %d\n", data->statistics_filename, data->alarm_time);

    // Fechar ficheiro de configuração;
    fclose(config_file);
}