/*  Grupo 039
 * Rafael Ribeiro   - 58193
 * Pedro Duque      - 52753
 * Francisco Santos - 59871
*/ 

#include <stdio.h>

#include "hosptime.h"
#include "main.h"
#include "memory.h"
#include "receptionist.h"
#include "synchronization.h"

int execute_receptionist(int receptionist_id, struct data_container* data, struct communication* comm, struct semaphores* sems) {
    // Memória alocada para uma admissão
    struct admission* ad = allocate_dynamic_memory(sizeof(struct admission));

    while (*data->terminate != 1) { // Enquanto o utilizador não pedir para terminar o programa
        receptionist_receive_admission(ad, data, comm, sems);
        if (ad->id != -1) { // Verificar que id != -1
            printf("[Receptionist %d] Recebi a admissão %d!\n", receptionist_id, ad->id);
            receptionist_process_admission(ad, receptionist_id, data, sems);
            receptionist_send_admission(ad, data, comm, sems);
        }
    }

    // Dealocar memória na estrutura admission
    deallocate_dynamic_memory(ad);
    return data->receptionist_stats[receptionist_id];
}

void receptionist_receive_admission(struct admission* ad, struct data_container* data, struct communication* comm, struct semaphores* sems) {
    if (*data->terminate == 1) {
        return;
    }
    consume_begin(sems->patient_receptionist);
    read_patient_receptionist_buffer(comm->patient_receptionist, data->buffers_size, ad);
    consume_end(sems->patient_receptionist);
}

void receptionist_process_admission(struct admission* ad, int receptionist_id, struct data_container* data, struct semaphores* sems) {
    ad->receptionist_time = get_time();
    ad->receiving_receptionist = receptionist_id;
    ad->status = 'R';
    data->receptionist_stats[receptionist_id]++;
    data->results[ad->id] = *ad;
}

void receptionist_send_admission(struct admission* ad, struct data_container* data, struct communication* comm, struct semaphores* sems) {
    produce_begin(sems->receptionist_doctor);
    write_receptionist_doctor_buffer(comm->receptionist_doctor, data->buffers_size, ad);
    produce_end(sems->receptionist_doctor);
}