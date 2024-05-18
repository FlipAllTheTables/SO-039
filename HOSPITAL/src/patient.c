/*  Grupo 039
 * Rafael Ribeiro   - 58193
 * Pedro Duque      - 52753
 * Francisco Santos - 59871
*/ 

#include <stdio.h>

#include "hospsignal.h"
#include "hosptime.h"
#include "patient.h"

int execute_patient(int patient_id, struct data_container* data, struct communication* comm, struct semaphores* sems) {
    // Definir qualquer processo "paciente" para ignorar o sinal SIGINT
    signal_ignore();

    // Memória alocada para uma admissão
    struct admission* ad = allocate_dynamic_memory(sizeof(struct admission));

    while (*data->terminate != 1) { // Enquanto o utilizador não pedir para terminar o programa
        patient_receive_admission(ad, patient_id, data, comm, sems);
        if (ad->id != -1) { // Verificar que id != -1
            printf("[Patient %d] Recebi a admissão %d!\n", patient_id, ad->id);
            patient_process_admission(ad, patient_id, data, sems);
            patient_send_admission(ad, data, comm, sems);
        }
    }

    // Dealocar memória na estrutura admission
    deallocate_dynamic_memory(ad);
    return data->patient_stats[patient_id];
}

void patient_receive_admission(struct admission* ad, int patient_id, struct data_container* data, struct communication* comm, struct semaphores* sems) {
    semaphore_lock(sems->terminate_mutex);
    if (*data->terminate == 1) {
        ad->id = -1;
        semaphore_unlock(sems->terminate_mutex);
        return;
    }
    semaphore_unlock(sems->terminate_mutex);
    consume_begin(sems->main_patient);
    read_main_patient_buffer(comm->main_patient, patient_id, data->buffers_size, ad);
    if (ad->id == -1) { // Se id for válido, por prodcon main_patient de volta como estava no início da operação
        // Função produce_end faz post nos semáforos em que se fez wait na função consume_being
        produce_end(sems->main_patient);
    } else { // Se id for válido, fazer consume_end ao prodcon main_patient
        consume_end(sems->main_patient);
    }
}

void patient_process_admission(struct admission* ad, int patient_id, struct data_container* data, struct semaphores* sems) {
    ad->patient_time = get_timespec();
    ad->receiving_patient = patient_id;
    ad->status = 'P';
    semaphore_lock(sems->patient_stats_mutex);
    data->patient_stats[patient_id]++;
    semaphore_unlock(sems->patient_stats_mutex);
    semaphore_lock(sems->results_mutex);
    data->results[ad->id] = *ad;
    semaphore_unlock(sems->results_mutex);
}

void patient_send_admission(struct admission* ad, struct data_container* data, struct communication* comm, struct semaphores* sems) {
    produce_begin(sems->patient_receptionist);
    write_patient_receptionist_buffer(comm->patient_receptionist, data->buffers_size, ad);
    produce_end(sems->patient_receptionist);
}