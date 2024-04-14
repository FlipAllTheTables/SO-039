/*  Grupo 039
 * Rafael Ribeiro   - 58193
 * Pedro Duque      - 52753
 * Francisco Santos - 59871
*/ 

#include <stdio.h>

#include "doctor.h"
#include "main.h"
#include "memory.h"

int execute_doctor(int doctor_id, struct data_container* data, struct communication* comm) {
    // Memória alocada para uma admissão
    struct admission* ad = allocate_dynamic_memory(sizeof(struct admission));

    while (*data->terminate != 1) { // Enquanto o utilizador não pedir para terminar o programa
        doctor_receive_admission(ad, doctor_id, data, comm);
        if (ad->id != -1) {
            printf("[Doctor %d] Recebi a admissão %d!\n", doctor_id, ad->id);
            doctor_process_admission(ad, doctor_id, data);
        }
    }

    // Dealocar memória na estrutura admission
    deallocate_dynamic_memory(ad);
    return data->patient_stats[doctor_id];
}

void doctor_receive_admission(struct admission* ad, int doctor_id, struct data_container* data, struct communication* comm) {
    if (*data->terminate == 1) {
        return;
    }
    read_main_patient_buffer(comm->main_patient, doctor_id, data->buffers_size, ad);
}

void doctor_process_admission(struct admission* ad, int doctor_id, struct data_container* data) {
    ad->receiving_doctor = doctor_id;
    ad->status = 'A';
    data->doctor_stats[doctor_id]++;
    data->results[ad->id] = *ad;
}