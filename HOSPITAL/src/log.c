/*  Grupo 039
 * Rafael Ribeiro   - 58193
 * Pedro Duque      - 52753
 * Francisco Santos - 59871
*/ 

#include <stdio.h>
#include <stdlib.h>

#include "hosptime.h"
#include "log.h"

void log_ad(struct data_container* data, int patient_id, int doctor_id) {
    // Ficheiro de log
    FILE* log_file;
    if ((log_file = fopen(data->log_filename, "a")) == NULL) { // Verificar que ficheiro foi aberto corretamente
        puts("Erro em criação de ficheiro de log.");
        exit(1);
    }

    // Escrever o momento em que a operação foi solicitada
    print_log_time(log_file);
    // Escrever a operação
    fprintf(log_file, "ad %d %d\n", patient_id, doctor_id);

    // Fechar ficheiro
    fclose(log_file);
}

void log_info(struct data_container* data, int ad_id) {
    // Ficheiro de log
    FILE* log_file;
    if ((log_file = fopen(data->log_filename, "a")) == NULL) { // Verificar que ficheiro foi aberto corretamente
        puts("Erro em criação de ficheiro de log.");
        exit(1);
    }

    // Escrever o momento em que a operação foi solicitada
    print_log_time(log_file);
    // Escrever a operação
    fprintf(log_file, "info %d\n", ad_id);

    // Fechar ficheiro
    fclose(log_file);
}

void log_status(struct data_container* data) {
    // Ficheiro de log
    FILE* log_file;
    if ((log_file = fopen(data->log_filename, "a")) == NULL) { // Verificar que ficheiro foi aberto corretamente
        puts("Erro em criação de ficheiro de log.");
        exit(1);
    }

    // Escrever o momento em que a operação foi solicitada
    print_log_time(log_file);
    // Escrever a operação
    fprintf(log_file, "status\n");

    // Fechar ficheiro
    fclose(log_file);
}

void log_help(struct data_container* data) {
    // Ficheiro de log
    FILE* log_file;
    if ((log_file = fopen(data->log_filename, "a")) == NULL) { // Verificar que ficheiro foi aberto corretamente
        puts("Erro em criação de ficheiro de log.");
        exit(1);
    }

    // Escrever o momento em que a operação foi solicitada
    print_log_time(log_file);
    // Escrever a operação
    fprintf(log_file, "help\n");

    // Fechar ficheiro
    fclose(log_file);
}

void log_end(struct data_container* data) {
    // Ficheiro de log
    FILE* log_file;
    if ((log_file = fopen(data->log_filename, "a")) == NULL) { // Verificar que ficheiro foi aberto corretamente
        puts("Erro em criação de ficheiro de log.");
        exit(1);
    }

    // Escrever o momento em que a operação foi solicitada
    print_log_time(log_file);
    // Escrever a operação
    fprintf(log_file, "end\n");

    // Fechar ficheiro
    fclose(log_file);
}

void print_log_time(FILE* log_file) {
    // Obter tempo real
    struct timespec current_time = get_timespec();

    // String que guarda o formato dia/mês/ano_hora:minuto:segundo
    char time_string[128];
    struct tm* time_format;

    time_format = localtime(&current_time.tv_sec);
    strftime(time_string, 128, "%d/%m/%Y_%H:%M:%S", time_format);
    fprintf(log_file, "%s.%.3ld ", time_string, current_time.tv_nsec / 1000000);
}