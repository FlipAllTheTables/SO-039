#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "memory.h"
#include "stats.h"

void print_statistics(struct data_container* data) {

    // Ficheiro de estatísticas
    FILE* stats_file;
    if ((stats_file = fopen(data->statistics_filename, "w")) == NULL) { // Verificar que ficheiro foi aberto corretamente
        puts("Erro em criação de ficheiro de estatísticas.");
        exit(1);
    }

    // Escrever as estatísticas de cada processo (paciente, rececionista e médico)
    fputs("Process Statistics:\n", stats_file);
    print_process_statistics(stats_file, data);

    fputs("\nAdmission Statistics:\n", stats_file);
    for (int i = 0; i < data->max_ads; i++) {
        if (data->results[i].id != -1) { // Verificar que admissão foi feita
            print_admission_statistics(stats_file, &data->results[i]);
        }
    }

    // Fechar ficheiro de estatísticas
    fclose(stats_file);
}

void print_process_statistics(FILE* stats_file, struct data_container* data) {
    // Escrever estatísticas de cada paciente no ficheiro de estatísticas
    for (int i = 0; i < data->n_patients; i++) {
        fprintf(stats_file, "\tPatient %d requested %d admissions!\n", i, data->patient_stats[i]);
    }
    // Escrever estatísticas de cada rececionista no ficheiro de estatísticas
    for (int i = 0; i < data->n_receptionists; i++) {
        fprintf(stats_file, "\tReceptionist %d requested %d admissions!\n", i, data->receptionist_stats[i]);
    }
    // Escrever estatísticas de cada rececionista no ficheiro de estatísticas
    for (int i = 0; i < data->n_doctors; i++) {
        fprintf(stats_file, "\tDoctor %d requested %d admissions!\n", i, data->doctor_stats[i]);
    }
}

void print_admission_statistics(FILE* stats_file, struct admission* ad) {
    fprintf(stats_file, "Admission: %d\n", ad->id);
    fprintf(stats_file, "Status: %c\n", ad->status);
    fprintf(stats_file, "Patient id: %d\n", ad->receiving_patient);
    fprintf(stats_file, "Receptionist id: %d\n", ad->receiving_receptionist);
    fprintf(stats_file, "Doctor id: %d\n", ad->receiving_doctor);
    fputs("\n", stats_file);
}