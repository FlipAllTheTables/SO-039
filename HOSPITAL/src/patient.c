/*  Grupo 039
 * Rafael Ribeiro   - 58193
 * Pedro Duque      - 52753
 * Francisco Santos - 59871
*/ 

#include <stdio.h>

#include "patient.h"
#include "main.h"
#include "memory.h"  

int execute_patient(int patient_id, struct data_container* data, struct communication* comm) {
    // Memória alocada para uma admissão
    struct admission* ad = allocate_dynamic_memory(sizeof(struct admission));

    while (*data->terminate != 1) { // Enquanto o utilizador não pedir para terminar o programa
        patient_receive_admission(ad, patient_id, data, comm);
        if (ad->id != -1) { // Verificar que id != -1
            printf("[Patient %d] Recebi a admissão %d!\n", patient_id, ad->id);
            patient_process_admission(ad, patient_id, data);
            patient_send_admission(ad, data, comm);
        }
    }

    // Dealocar memória na estrutura admission
    deallocate_dynamic_memory(ad);
    return data->patient_stats[patient_id];
}

void patient_receive_admission(struct admission* ad, int patient_id, struct data_container* data, struct communication* comm) {
    if (*data->terminate == 1) {
        return;
    }
    read_main_patient_buffer(comm->main_patient, patient_id, data->buffers_size, ad);
}

void patient_process_admission(struct admission* ad, int patient_id, struct data_container* data) {
    ad->receiving_patient = patient_id;
    ad->status = 'P';
    (data->patient_stats[patient_id]++);
    data->results[ad->id] = *ad;
}

void patient_send_admission(struct admission* ad, struct data_container* data, struct communication* comm) {
    write_patient_receptionist_buffer(comm->patient_receptionist, data->buffers_size, ad);
}