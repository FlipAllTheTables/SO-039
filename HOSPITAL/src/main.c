#include <stdio.h>
#include <stdlib.h>

#include "../include/main.h"
#include "../include/memory.h"

int main(int argc, char *argv[]) {
    //init data structures
    struct data_container* data = allocate_dynamic_memory(sizeof(struct data_container));
    struct communication* comm = allocate_dynamic_memory(sizeof(struct communication));
    comm->main_patient = allocate_dynamic_memory(sizeof(struct circular_buffer));
    comm->patient_receptionist = allocate_dynamic_memory(sizeof(struct rnd_access_buffer));
    comm->receptionist_doctor = allocate_dynamic_memory(sizeof(struct circular_buffer));

    //execute main code
    main_args(argc, argv, data);
    allocate_dynamic_memory_buffers(data);
    create_shared_memory_buffers(data, comm);
    launch_processes(data, comm);
    user_interaction(data, comm);
    
    //release memory before terminating
    deallocate_dynamic_memory(data);
    deallocate_dynamic_memory(comm->main_patient);
    deallocate_dynamic_memory(comm->patient_receptionist);
    deallocate_dynamic_memory(comm->receptionist_doctor);
    deallocate_dynamic_memory(comm);
}

void main_args(int argc, char* argv[], struct data_container* data) {
    if (argc != 6) { // Verificar se o número de argumentos é 6, incluindo o nome do executável hOSpital
        puts("Número de argumentos incorreto.");
        puts("Exemplo de utilização:\n./hOSpital max_ads buffers_size n_patients n_receptionists n_doctors");
        exit(1);
    }
    // Inserir argumentos na strutura data_container
    data->max_ads = atoi(argv[1]);
    data->buffers_size = atoi(argv[2]);
    data->n_patients = atoi(argv[3]);
    data->n_receptionists = atoi(argv[4]);
    data->n_doctors = atoi(argv[5]);
}

void allocate_dynamic_memory_buffers(struct data_container* data) {
    // Alocar memória para pids e stats de pacientes
    data->patient_pids = allocate_dynamic_memory(data->n_patients * sizeof(int));
    data->patient_stats = allocate_dynamic_memory(data->n_patients * sizeof(int));

    // Alocar memória para pids e stats de rececionistas
    data->receptionist_pids = allocate_dynamic_memory(data->n_receptionists * sizeof(int));
    data->receptionist_stats = allocate_dynamic_memory(data->n_receptionists * sizeof(int));

    // Alocar memória para pids e stats de médicos
    data->doctor_pids = allocate_dynamic_memory(data->n_doctors * sizeof(int));
    data->doctor_stats = allocate_dynamic_memory(data->n_doctors * sizeof(int));
}

void create_shared_memory_buffers(struct data_container* data, struct communication* comm) {
    
}

void launch_processes(struct data_container* data, struct communication* comm) {
    for (int i = 0; i < data->n_patients; i++) { // Iniciar N pacientes e guardar pids em data->patient_pids
        int pid = launch_patient(i, data, comm);
        data->patient_pids[i] = pid;
    }
    for (int i = 0; i < data->n_receptionists; i++) { // Iniciar N rececionistas e guardar pids em data->receptionists_pids
        int pid = launch_receptionist(i, data, comm);
        data->receptionist_pids[i] = pid;
    }
    for (int i = 0; i < data->n_doctors; i++) { // Iniciar N médicos e guardar pids em data->doctor_pids
        int pid = launch_doctor(i, data, comm);
        data->doctor_pids[i] = pid;
    }
}