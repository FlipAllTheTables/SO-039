#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/main.h"
#include "../include/memory.h"
#include "../include/process.h"

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
    // Inicializar memória partilhada da estrutura data_container
    data->results = create_shared_memory(STR_SHM_RESULTS, MAX_RESULTS);
    data->terminate = create_shared_memory(STR_SHM_TERMINATE, sizeof(int));

    *data->terminate = 0;

    // Inicializar memória partilhada entre main e paciente
    comm->main_patient->buffer = create_shared_memory(STR_SHM_MAIN_PATIENT_BUFFER, data->buffers_size * sizeof(struct admission));
    comm->main_patient->ptrs = create_shared_memory(STR_SHM_MAIN_PATIENT_PTR, sizeof(struct pointers));

    // Inicializar memória partilhada entre paciente e rececionista
    comm->patient_receptionist->buffer = create_shared_memory(STR_SHM_PATIENT_RECEPT_BUFFER, data->buffers_size * sizeof(struct admission));
    comm->patient_receptionist->ptrs = create_shared_memory(STR_SHM_PATIENT_RECEPT_PTR, data->buffers_size * sizeof(int));

    // Inicializar memória partilhada entre rececionista e médico
    comm->receptionist_doctor->buffer = create_shared_memory(STR_SHM_RECEPT_DOCTOR_BUFFER, data->buffers_size * sizeof(struct admission));
    comm->receptionist_doctor->ptrs = create_shared_memory(STR_SHM_RECEPT_DOCTOR_PTR, sizeof(struct pointers));
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

void user_interaction(struct data_container* data, struct communication* comm) {
    int ad_counter = 0;

    // Número máximo de caracteres é 5 incluíndo '\0' ("info\0" e "help\0")
    char user_input[5];

    // Lista de ações que aparece no início de executação
    puts("[Main] Ações disponíveis:");
    puts("[Main]  ad paciente médico - criar uma nova admissão");
    puts("[Main]  info id - consultar o estado de duma admissão");
    puts("[Main]  help - imprime informação sobre as ações disponíveis");
    puts("[Main]  end - termina a executação de hOSpital\n");

    while(1) { // Loop infinito até utilizador escrever "end"

        // Pedir input the utilizador
        printf("[Main] Introduzir ação: ");
        scanf("%s", &user_input);

        if (strcmp(user_input, "ad") == 0) {
            
        } else if (strcmp(user_input, "info") == 0) {

        } else if (strcmp(user_input, "help") == 0) {
            puts("[Main] Ações disponíveis:");
            puts("[Main]  ad paciente médico - criar uma nova admissão");
            puts("[Main]  info id - consultar o estado de duma admissão");
            puts("[Main]  help - imprime informação sobre as ações disponíveis");
            puts("[Main]  end - termina a executação de hOSpital\n");
        } else if (strcmp(user_input, "end")) {

        } else { // Comando mal formatado não existente
            puts("[Main] Ação não reconhecida, insira 'help' para assistência");
        }
    }
}