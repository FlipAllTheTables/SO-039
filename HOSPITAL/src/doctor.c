/*  Grupo 039
 * Rafael Ribeiro   - 58193
 * Pedro Duque      - 52753
 * Francisco Santos - 59871
*/ 

#include <stdio.h>

#include "doctor.h"
#include "hosptime.h"

int execute_doctor(int doctor_id, struct data_container* data, struct communication* comm, struct semaphores* sems) {
    // Memória alocada para uma admissão
    struct admission* ad = allocate_dynamic_memory(sizeof(struct admission));

    while (*data->terminate != 1) { // Enquanto o utilizador não pedir para terminar o programa
        doctor_receive_admission(ad, doctor_id, data, comm, sems);
        if (ad->id != -1) {
            printf("[Doctor %d] Recebi a admissão %d!\n", doctor_id, ad->id);
            doctor_process_admission(ad, doctor_id, data, sems);
        }
    }

    // Dealocar memória na estrutura admission
    deallocate_dynamic_memory(ad);
    return data->doctor_stats[doctor_id];
}

void doctor_receive_admission(struct admission* ad, int doctor_id, struct data_container* data, struct communication* comm, struct semaphores* sems) {
    if (*data->terminate == 1) {
        return;
    }
    consume_begin(sems->receptionist_doctor);
    read_receptionist_doctor_buffer(comm->receptionist_doctor, doctor_id, data->buffers_size, ad);
    if (ad->id == -1) { // Se id for inválido, por prodcon receptionist_doctor de volta como estava no início da operação
        // Função produce_end faz post nos semáforos em que se fez wait na função consume_being
        produce_end(sems->receptionist_doctor);
    } else { // Se id for válido, fazer consume_end ao prodcon receptionist_doctor
        consume_end(sems->receptionist_doctor);
    }
}

void doctor_process_admission(struct admission* ad, int doctor_id, struct data_container* data, struct semaphores* sems) {
    ad->doctor_time = get_timespec();
    ad->receiving_doctor = doctor_id;
    if (ad->id < data->max_ads) { // Se admissão não estiver acima do limite diário, incrementar contador e inserir estado 'A'
        ad->status = 'A';
        data->doctor_stats[doctor_id]++;
    } else { // Caso contrário inserir estado N e não incrementar contador
        ad->status = 'N';
    }
    data->results[ad->id] = *ad;
}