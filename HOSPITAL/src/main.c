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
    data->results = create_shared_memory(STR_SHM_RESULTS, MAX_RESULTS * sizeof(struct admission));
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

    // Imprimir lista de ações que aparece no início de executação
    puts("[Main] Ações disponíveis:");
    puts("[Main]  ad paciente médico - criar uma nova admissão");
    puts("[Main]  info id - consultar o estado de duma admissão");
    puts("[Main]  help - imprime informação sobre as ações disponíveis");
    puts("[Main]  end - termina a executação de hOSpital\n");

    while(1) { // Loop infinito até utilizador escrever "end"

        // Pedir input the utilizador
        printf("[Main] Introduzir ação: ");
        scanf("%s", user_input);

        if (strcmp(user_input, "ad") == 0) {
            create_request(&ad_counter, data, comm);

        } else if (strcmp(user_input, "info") == 0) {
            read_info(data);

        } else if (strcmp(user_input, "help") == 0) {
            puts("[Main] Ações disponíveis:");
            puts("[Main]  ad paciente médico - criar uma nova admissão");
            puts("[Main]  info id - consultar o estado de duma admissão");
            puts("[Main]  help - imprime informação sobre as ações disponíveis");
            puts("[Main]  end - termina a executação de hOSpital\n");

        } else if (strcmp(user_input, "end")) {
            puts("Fixe");
            return;

        } else { // Comando mal formatado não existente
            puts("[Main] Ação não reconhecida, insira 'help' para assistência");
        }
    }
}

void create_request(int* ad_counter, struct data_container* data, struct communication* comm) {
    // Obter valores de id de paciente e médico do comando "ad paciente médico"
    int patient_id, doctor_id;
    scanf("%d", &patient_id);
    scanf("%d", &doctor_id);

    // Inicializar admissão
    struct admission* ad = allocate_dynamic_memory(sizeof(struct admission));
    ad->id = *ad_counter;
    ad->requesting_patient = patient_id;
    ad->requested_doctor = doctor_id;
    ad->status = 'M'; // Estado inicial de uma admissão criada por Main

    // Escrever admissão na memória partilhada entre main e paciente
    write_main_patient_buffer(comm->main_patient, data->buffers_size, ad);

    // Imprimir ID da admissão criada e incrementar contador de admissões
    printf("[Main] A admissão %d foi criada!", ad_counter);
    *ad_counter++;
}

void read_info(struct data_container* data) {

}

void print_status(struct data_container* data) {
    // Imprimir número máximo de admissões e tamanho de buffers
    printf("%d\n", data->max_ads);
    printf("%d\n", data->buffers_size);

    // Imprimir número de pacientes, rececionistas e
    printf("%d\n", data->n_patients);
    printf("%d\n", data->n_receptionists);
    printf("%d\n", data->n_doctors);

    // Imprimir arrays de pids de pacientes, rececionistas e médicos no formato pedido
    printf("[");
    for (int i = 0; i < data->n_patients; i++) {
        printf("%d", data->patient_pids[i]);
        if (i < data->n_patients - 1) {
            printf(", ");
        }
    }
    printf("]\n");

    printf("[");
    for (int i = 0; i < data->n_receptionists; i++) {
        printf("%d", data->receptionist_pids[i]);
        if (i < data->n_receptionists - 1) {
            printf(", ");
        }
    }
    printf("]\n");

    printf("[");
    for (int i = 0; i < data->n_doctors; i++) {
        printf("%d", data->doctor_pids[i]);
        if (i < data->n_doctors - 1) {
            printf(", ");
        }
    }
    printf("]\n");

    // Imprimir arrays de stats de pacientes, rececionistas e médicos no formato pedido
    printf("[");
    for (int i = 0; i < data->n_patients; i++) {
        printf("%d", data->patient_stats[i]);
        if (i < data->n_patients - 1) {
            printf(", ");
        }
    }
    printf("]\n");

    printf("[");
    for (int i = 0; i < data->n_receptionists; i++) {
        printf("%d", data->receptionist_stats[i]);
        if (i < data->n_receptionists - 1) {
            printf(", ");
        }
    }
    printf("]\n");

    printf("[");
    for (int i = 0; i < data->n_doctors; i++) {
        printf("%d", data->doctor_stats[i]);
        if (i < data->n_doctors - 1) {
            printf(", ");
        }
    }
    printf("]\n");

    // Imprimir array de admissões
    // TODO

}

void write_statistics(struct data_container* data) {
    // Escrever número de admissões requeridas por cada paciente
    for (int i = 0; i < data->n_patients; i++) {
        printf("[Main] O paciente %d requeriu %d admissões!\n", i, data->patient_stats[i]);
    }

    // Escrever número de admissões realizadas por cada rececionista
    for (int i = 0; i < data->n_receptionists; i++) {
        printf("[Main] O rececionista %d requeriu %d admissões!\n", i, data->receptionist_stats[i]);
    }

    // Escrever número de admissões atendidas por cada médico
    for (int i = 0; i < data->n_doctors; i++) {
        printf("[Main] O médico %d requeriu %d admissões!\n", i, data->doctor_stats[i]);
    }
}