/*  Grupo 039
 * Rafael Ribeiro   - 58193
 * Pedro Duque      - 52753
 * Francisco Santos - 59871
*/ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include "main.h"
#include "memory.h"
#include "process.h"

int main(int argc, char *argv[]) {
    //init data structures
    struct data_container* data = allocate_dynamic_memory(sizeof(struct data_container));
    struct communication* comm = allocate_dynamic_memory(sizeof(struct communication));
    comm->main_patient = allocate_dynamic_memory(sizeof(struct circular_buffer));
    comm->patient_receptionist = allocate_dynamic_memory(sizeof(struct rnd_access_buffer));
    comm->receptionist_doctor = allocate_dynamic_memory(sizeof(struct circular_buffer));

    // init semaphore data structure
    struct semaphores* sems = allocate_dynamic_memory(sizeof(struct semaphores));
    sems->main_patient = allocate_dynamic_memory(sizeof(struct prodcons));
    sems->patient_receptionist = allocate_dynamic_memory(sizeof(struct prodcons));
    sems->receptionist_doctor = allocate_dynamic_memory(sizeof(struct prodcons));

    //execute main code
    main_args(argc, argv, data);
    allocate_dynamic_memory_buffers(data);
    create_shared_memory_buffers(data, comm);
    create_semaphores(data, sems);
    launch_processes(data, comm, sems);
    user_interaction(data, comm, sems);

    //release memory before terminating
    deallocate_dynamic_memory(data);
    deallocate_dynamic_memory(comm->main_patient);
    deallocate_dynamic_memory(comm->patient_receptionist);
    deallocate_dynamic_memory(comm->receptionist_doctor);
    deallocate_dynamic_memory(comm);
    allocate_dynamic_memory(sems->main_patient);
    allocate_dynamic_memory(sems-> patient_receptionist);
    allocate_dynamic_memory(sems-> receptionist_doctor);
    allocate_dynamic_memory(sems);
}

void main_args(int argc, char* argv[], struct data_container* data) {
    if (argc != 6) { // Verificar se o número de argumentos é 6, incluindo o nome do executável hOSpital
        puts("Número de argumentos incorreto.");
        puts("Exemplo de utilização:\n./hOSpital max_ads buffers_size n_patients n_receptionists n_doctors");
        exit(1);
    }
    // Inserir argumentos na strutura data_container
    if (atoi(argv[1]) > MAX_RESULTS) {
        data->max_ads = MAX_RESULTS;
    } else {
        data->max_ads = atoi(argv[1]);
    }
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
    data->results = create_shared_memory(STR_SHM_RESULTS, data->max_ads * sizeof(struct admission));
    // Certificar que todas as admissões dentro de results começam com id = -1
    for (int i = 0; i < data->max_ads; i++) {
        data->results[i].id = -1;
    }

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
        data->patient_pids[i] = launch_patient(i, data, comm);
    }
    for (int i = 0; i < data->n_receptionists; i++) { // Iniciar N rececionistas e guardar pids em data->receptionists_pids
        data->receptionist_pids[i] = launch_receptionist(i, data, comm);
    }
    for (int i = 0; i < data->n_doctors; i++) { // Iniciar N médicos e guardar pids em data->doctor_pids
        data->doctor_pids[i] = launch_doctor(i, data, comm);
    }
}

void user_interaction(struct data_container* data, struct communication* comm) {
    int ad_counter = 0;

    // String que guarda o input do utilizador
    char user_input[10];

    // Imprimir lista de ações que aparece no início de executação
    puts("[Main] Ações disponíveis:");
    puts("[Main]  ad paciente médico - criar uma nova admissão");
    puts("[Main]  info id - consultar o estado de duma admissão");
    puts("[Main]  status - apresentar o estado atual");
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

        } else if (strcmp(user_input, "status") == 0) {
            print_status(data);

        } else if (strcmp(user_input, "help") == 0) {
            puts("[Main] Ações disponíveis:");
            puts("[Main]  ad paciente médico - criar uma nova admissão");
            puts("[Main]  info id - consultar o estado de duma admissão");
            puts("[Main]  status - apresentar o estado atual");
            puts("[Main]  help - imprime informação sobre as ações disponíveis");
            puts("[Main]  end - termina a executação de hOSpital\n");

        } else if (strcmp(user_input, "end") == 0) {
            end_execution(data, comm);
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
    if (*ad_counter < data->max_ads) { // Verificar que ainda pode ser criada uma admissão
        printf("[Main] A criar a admissão %d para o paciente %d destinada ao médico %d!\n", *ad_counter, patient_id, doctor_id);
        // Inicializar admissão
        struct admission* ad = allocate_dynamic_memory(sizeof(struct admission));
        ad->id = *ad_counter;
        ad->requesting_patient = patient_id;
        ad->requested_doctor = doctor_id;
        ad->status = 'M'; // Estado inicial de uma admissão criada por Main

        // Escrever admissão na memória partilhada entre main e paciente
        write_main_patient_buffer(comm->main_patient, data->buffers_size, ad);

        // Imprimir ID da admissão criada e incrementar contador de admissões
        printf("[Main] A admissão %d foi criada!\n", ad->id);
        data->results[ad->id] = *ad;
        (*ad_counter)++;
        deallocate_dynamic_memory(ad);
    } else { // Número máximo de admissões alcançado, não se cria uma admissão nova
        puts("[Main] O número máximo de admissões foi alcançado!");
    }
}

void read_info(struct data_container* data) {
    // Obter valor de id da admissão pedida pelo utilizador
    int ad_id;
    scanf("%d", &ad_id);

    // Imprimir mensagem de estado na consola. Mensagem diferente dependendo do estado da admissão
    if (ad_id < data->max_ads) {
        if (data->results[ad_id].id == ad_id) {
            char status = data->results[ad_id].status;
            printf("[Main] A admissão %d com estado %c requisitada pelo paciente %d ao médico %d", ad_id, status, data->results[ad_id].requesting_patient, data->results[ad_id].requested_doctor);
            if (status == 'M') {
                printf(", foi enviada ao paciente");
            } else {
                printf(", foi recebida pelo paciente %d", data->results[ad_id].receiving_patient);
                if (status == 'P') {
                    printf(", e enviada ao rececionista");
                } else {
                    printf(", admitida pelo rececionista %d", data->results[ad_id].receiving_receptionist);
                    if (status == 'R') {
                        printf(", e enviada ao médico");
                    } else {
                        printf(", e concluída pelo médico %d", data->results[ad_id].receiving_doctor);
                    }
                }
            }
        } else {
            printf("[Main] A admissão %d ainda não existe", ad_id);
        }
    } else {
        printf("[Main] A admissão %d possui um id maior ou igual ao máximo de atendimentos previstos para a execução (%d)", ad_id, data->max_ads);
    }
    printf("!\n");
}

void print_status(struct data_container* data) {
    // Imprimir número máximo de admissões e tamanho de buffers
    printf("[Main] max_ads: %d\n", data->max_ads);
    printf("[Main] buffers_size: %d\n", data->buffers_size);

    // Imprimir número de pacientes, rececionistas e
    printf("[Main] n_patients: %d\n", data->n_patients);
    printf("[Main] n_receptionists: %d\n", data->n_receptionists);
    printf("[Main] n_doctors: %d\n", data->n_doctors);

    // Imprimir arrays de pids de pacientes, rececionistas e médicos no formato pedido
    printf("[Main] patient_pids: [");
    for (int i = 0; i < data->n_patients; i++) {
        printf("%d", data->patient_pids[i]);
        if (i < data->n_patients - 1) {
            printf(", ");
        }
    }
    printf("]\n");

    printf("[Main] receptionist_pids: [");
    for (int i = 0; i < data->n_receptionists; i++) {
        printf("%d", data->receptionist_pids[i]);
        if (i < data->n_receptionists - 1) {
            printf(", ");
        }
    }
    printf("]\n");

    printf("[Main] doctor_pids: [");
    for (int i = 0; i < data->n_doctors; i++) {
        printf("%d", data->doctor_pids[i]);
        if (i < data->n_doctors - 1) {
            printf(", ");
        }
    }
    printf("]\n");

    // Imprimir arrays de stats de pacientes, rececionistas e médicos no formato pedido
    printf("[Main] patient_stats: [");
    for (int i = 0; i < data->n_patients; i++) {
        printf("%d", data->patient_stats[i]);
        if (i < data->n_patients - 1) {
            printf(", ");
        }
    }
    printf("]\n");

    printf("[Main] receptionist_stats: [");
    for (int i = 0; i < data->n_receptionists; i++) {
        printf("%d", data->receptionist_stats[i]);
        if (i < data->n_receptionists - 1) {
            printf(", ");
        }
    }
    printf("]\n");

    printf("[Main] doctor_stats [");
    for (int i = 0; i < data->n_doctors; i++) {
        printf("%d", data->doctor_stats[i]);
        if (i < data->n_doctors - 1) {
            printf(", ");
        }
    }
    printf("]\n");

    // Imprimir array de IDs de admissões
    printf("[Main] results: [");
    for (int i = 0; i < data->max_ads; i++) {
        printf("%d", data->results[i].id);
        if (i < data->max_ads - 1) {
            printf(", ");
        }
    }
    printf("]\n");

    // Imprimir estado terminate
    printf("[Main] terminate: %d\n\n", *data->terminate);

}

void end_execution(struct data_container* data, struct communication* comm) {
    // Por flag terminate da estrutura data para 1
    *data->terminate = 1;
    wait_processes(data);
    write_statistics(data);
    destroy_memory_buffers(data, comm);
}

void wait_processes(struct data_container* data) {
    // Para cada pid de paciente, chamar wait_process e escrever resultado no array data->pacient_stats
    for (int i = 0; i < data->n_patients; i++) {
        data->patient_stats[i] = wait_process(data->patient_pids[i]);
    }

    // Para cada pid de rececionista, chamar wait_process e escrever resultado no array data->receptionist_stats
    for (int i = 0; i < data->n_receptionists; i++) {
        data->receptionist_stats[i] = wait_process(data->receptionist_pids[i]);
    }

    // Para cada pid de médico, chamar wait_process e escrever resultado no array data->doctor_stats
    for (int i = 0; i < data->n_doctors; i++) {
        data->doctor_stats[i] = wait_process(data->doctor_pids[i]);
    }
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

void destroy_memory_buffers(struct data_container* data, struct communication* comm) {
    // TODO usar getuid()

    // Destroir memória partilhada da estrutura communication
    destroy_shared_memory(STR_SHM_MAIN_PATIENT_BUFFER, comm->main_patient->buffer, data->buffers_size * sizeof(struct admission));
    destroy_shared_memory(STR_SHM_MAIN_PATIENT_PTR, comm->main_patient->ptrs, sizeof(struct pointers));

    destroy_shared_memory(STR_SHM_PATIENT_RECEPT_BUFFER, comm->patient_receptionist->buffer, data->buffers_size * sizeof(struct admission));
    destroy_shared_memory(STR_SHM_PATIENT_RECEPT_PTR, comm->patient_receptionist->ptrs, data->buffers_size * sizeof(int));

    destroy_shared_memory(STR_SHM_RECEPT_DOCTOR_BUFFER, comm->receptionist_doctor->buffer, data->buffers_size * sizeof(struct admission));
    destroy_shared_memory(STR_SHM_RECEPT_DOCTOR_PTR, comm->receptionist_doctor->ptrs, sizeof(struct pointers));

    // Destroir memória partilhada da estrutura data_container
    destroy_shared_memory(STR_SHM_RESULTS, data->results, MAX_RESULTS * sizeof(struct admission));
    destroy_shared_memory(STR_SHM_TERMINATE, data->terminate, sizeof(int));

    // Libertar memória dinâmica alocada na estrutura data_container
    deallocate_dynamic_memory(data->patient_pids);
    deallocate_dynamic_memory(data->patient_stats);
    deallocate_dynamic_memory(data->receptionist_pids);
    deallocate_dynamic_memory(data->receptionist_stats);
    deallocate_dynamic_memory(data->doctor_pids);
    deallocate_dynamic_memory(data->doctor_stats);
}