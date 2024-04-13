#include <stdio.h>

#include "../include/receptionist.h"
#include "../include/memory.h"

int execute_receptionist(int receptionist_id, struct data_container* data, struct communication* comm) {
    // Memória alocada para uma admissão
    struct admission* ad = allocate_dynamic_memory(sizeof(struct admission));

    while (*data->terminate != 1) { // Enquanto o utilizador não pedir para terminar o programa
        receptionist_receive_admission(ad, data, comm);
        if (ad->id != -1) { // Verificar que id != -1
            receptionist_process_admission(ad, receptionist_id, data);
            receptionist_send_admission(ad, data, comm);
        }
    }

    // Dealocar memória na estrutura admission
    deallocate_dynamic_memory(ad);
    return data->receptionist_stats[receptionist_id];
}

void receptionist_receive_admission(struct admission* ad, struct data_container* data, struct communication* comm) {
    if (*data->terminate == 1) {
        return;
    }
    read_patient_receptionist_buffer(comm->patient_receptionist, data->buffers_size, ad);
}

void receptionist_process_admission(struct admission* ad, int receptionist_id, struct data_container* data) {
    ad->receiving_patient = receptionist_id;
    ad->status = 'R';
    data->receptionist_stats[receptionist_id]++;
    data->results[ad->id] = *ad;
}

void receptionist_send_admission(struct admission* ad, struct data_container* data, struct communication* comm) {
    write_receptionist_doctor_buffer(comm->receptionist_doctor, data->buffers_size, ad);
}