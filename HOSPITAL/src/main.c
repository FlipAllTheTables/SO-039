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

#include "configuration.h"
#include "hosptime.h"
#include "log.h"
#include "main.h"
#include "process.h"
#include "stats.h"

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
    deallocate_dynamic_memory(sems->main_patient);
    deallocate_dynamic_memory(sems-> patient_receptionist);
    deallocate_dynamic_memory(sems-> receptionist_doctor);
    deallocate_dynamic_memory(sems);
}

void main_args(int argc, char* argv[], struct data_container* data) {
    if (argc != 2) { // Verificar se o número de argumentos é 2, incluindo o nome do executável hOSpital
        puts("Número de argumentos incorreto. Exemplo de utilização:\n./hOSpital config.txt");
        exit(1);
    }
    // Inserir argumentos na strutura data_container
    read_config_information(argv[1], data);

    // Apagar conteúdos do ficheiro log, se este existir
    FILE* log_file;
    if ((log_file = fopen(data->log_filename, "w")) == NULL) {
        puts("Erro em criação de ficheiro log");
        exit(1);
    }
    fclose(log_file);
}

void allocate_dynamic_memory_buffers(struct data_container* data) {
    // Alocar memória para pids de pacientes, rececionistas e médicos
    data->patient_pids = allocate_dynamic_memory(data->n_patients * sizeof(int));
    data->receptionist_pids = allocate_dynamic_memory(data->n_receptionists * sizeof(int));
    data->doctor_pids = allocate_dynamic_memory(data->n_doctors * sizeof(int));
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

    // Inicializar memória partilhada de stats
    data->patient_stats = create_shared_memory(STR_SHM_PATIENT_STATS, data->n_patients * sizeof(int));
    data->receptionist_stats = create_shared_memory(STR_SHM_RECEPT_STATS, data->n_receptionists * sizeof(int));
    data->doctor_stats = create_shared_memory(STR_SHM_DOCTOR_STATS, data->n_doctors * sizeof(int));

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

void launch_processes(struct data_container* data, struct communication* comm, struct semaphores* sems) {
    for (int i = 0; i < data->n_patients; i++) { // Iniciar N pacientes e guardar pids em data->patient_pids
        data->patient_pids[i] = launch_patient(i, data, comm, sems);
    }
    for (int i = 0; i < data->n_receptionists; i++) { // Iniciar N rececionistas e guardar pids em data->receptionists_pids
        data->receptionist_pids[i] = launch_receptionist(i, data, comm, sems);
    }
    for (int i = 0; i < data->n_doctors; i++) { // Iniciar N médicos e guardar pids em data->doctor_pids
        data->doctor_pids[i] = launch_doctor(i, data, comm, sems);
    }
}

void user_interaction(struct data_container* data, struct communication* comm, struct semaphores* sems) {
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
            produce_begin(sems->main_patient);
            create_request(&ad_counter, data, comm, sems);
            produce_end(sems->main_patient);

        } else if (strcmp(user_input, "info") == 0) {
            read_info(data, sems);

        } else if (strcmp(user_input, "status") == 0) {
            print_status(data, sems);

        } else if (strcmp(user_input, "help") == 0) {
            log_help(data);
            puts("[Main] Ações disponíveis:");
            puts("[Main]  ad paciente médico - criar uma nova admissão");
            puts("[Main]  info id - consultar o estado de duma admissão");
            puts("[Main]  status - apresentar o estado atual");
            puts("[Main]  help - imprime informação sobre as ações disponíveis");
            puts("[Main]  end - termina a executação de hOSpital\n");

        } else if (strcmp(user_input, "end") == 0) {
            end_execution(data, comm, sems);
            return;

        } else { // Comando mal formatado não existente
            puts("[Main] Ação não reconhecida, insira 'help' para assistência");
        }
    }
}

void create_request(int* ad_counter, struct data_container* data, struct communication* comm, struct semaphores* sems) {
    // Obter valores de id de paciente e médico do comando "ad paciente médico"
    int patient_id, doctor_id;
    scanf("%d", &patient_id);
    scanf("%d", &doctor_id);
    log_ad(data, patient_id, doctor_id);
    if (*ad_counter < data->max_ads) { // Verificar que ainda pode ser criada uma admissão
        printf("[Main] A criar a admissão %d para o paciente %d destinada ao médico %d!\n", *ad_counter, patient_id, doctor_id);
        // Inicializar admissão
        struct admission* ad = allocate_dynamic_memory(sizeof(struct admission));
        ad->id = *ad_counter;
        ad->requesting_patient = patient_id;
        ad->requested_doctor = doctor_id;
        ad->status = 'M'; // Estado inicial de uma admissão criada por Main
        ad->create_time = get_timespec();

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

void read_info(struct data_container* data, struct semaphores* sems) {
    // Obter valor de id da admissão pedida pelo utilizador
    int ad_id;
    scanf("%d", &ad_id);
    log_info(data, ad_id);
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

void print_status(struct data_container* data, struct semaphores* sems) {
    log_status(data);

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

void end_execution(struct data_container* data, struct communication* comm, struct semaphores* sems) {
    log_end(data);

    // Por flag terminate da estrutura data para 1
    *data->terminate = 1;
    wakeup_processes(data, sems);
    wait_processes(data);
    write_statistics(data);
    destroy_memory_buffers(data, comm);
    destroy_semaphores(sems);
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
    print_statistics(data);
}

void destroy_memory_buffers(struct data_container* data, struct communication* comm) {
    // TODO usar getuid()

    // Destruir memória partilhada da estrutura communication
    destroy_shared_memory(STR_SHM_MAIN_PATIENT_BUFFER, comm->main_patient->buffer, data->buffers_size * sizeof(struct admission));
    destroy_shared_memory(STR_SHM_MAIN_PATIENT_PTR, comm->main_patient->ptrs, sizeof(struct pointers));

    destroy_shared_memory(STR_SHM_PATIENT_RECEPT_BUFFER, comm->patient_receptionist->buffer, data->buffers_size * sizeof(struct admission));
    destroy_shared_memory(STR_SHM_PATIENT_RECEPT_PTR, comm->patient_receptionist->ptrs, data->buffers_size * sizeof(int));

    destroy_shared_memory(STR_SHM_RECEPT_DOCTOR_BUFFER, comm->receptionist_doctor->buffer, data->buffers_size * sizeof(struct admission));
    destroy_shared_memory(STR_SHM_RECEPT_DOCTOR_PTR, comm->receptionist_doctor->ptrs, sizeof(struct pointers));

    // Destruir memória partilhada da estrutura data_container
    destroy_shared_memory(STR_SHM_RESULTS, data->results, data->max_ads * sizeof(struct admission));
    destroy_shared_memory(STR_SHM_TERMINATE, data->terminate, sizeof(int));
    destroy_shared_memory(STR_SHM_PATIENT_STATS, data->patient_stats, data->n_patients * sizeof(int));
    destroy_shared_memory(STR_SHM_RECEPT_STATS, data->receptionist_stats, data->n_receptionists * sizeof(int));
    destroy_shared_memory(STR_SHM_DOCTOR_STATS, data->doctor_stats, data->n_doctors * sizeof(int));

    // Libertar memória dinâmica alocada na estrutura data_container
    deallocate_dynamic_memory(data->patient_pids);
    deallocate_dynamic_memory(data->receptionist_pids);
    deallocate_dynamic_memory(data->doctor_pids);
}

void create_semaphores(struct data_container* data, struct semaphores* sems) {
    // Criar prodcon main_patient
    sems->main_patient->full = semaphore_create(STR_SEM_MAIN_PATIENT_FULL, 0);
    sems->main_patient->empty = semaphore_create(STR_SEM_MAIN_PATIENT_EMPTY, data->buffers_size);
    sems->main_patient->mutex = semaphore_create(STR_SEM_MAIN_PATIENT_MUTEX, 1);

    // Criar prodcon patient_receptionist
    sems->patient_receptionist->full = semaphore_create(STR_SEM_PATIENT_RECEPT_FULL, 0);
    sems->patient_receptionist->empty = semaphore_create(STR_SEM_PATIENT_RECEPT_EMPTY, data->buffers_size);
    sems->patient_receptionist->mutex = semaphore_create(STR_SEM_PATIENT_RECEPT_MUTEX, 1);

    // Criar prodcon receptionist_doctor
    sems->receptionist_doctor->full = semaphore_create(STR_SEM_RECEPT_DOCTOR_FULL, 0);
    sems->receptionist_doctor->empty = semaphore_create(STR_SEM_RECEPT_DOCTOR_EMPTY, data->buffers_size);
    sems->receptionist_doctor->mutex = semaphore_create(STR_SEM_RECEPT_DOCTOR_MUTEX, 1);

    // Criar semaphores mutex
    sems->patient_stats_mutex = semaphore_create(STR_SEM_PATIENT_STATS_MUTEX, 1);
    sems->receptionist_stats_mutex = semaphore_create(STR_SEM_RECEPT_STATS_MUTEX, 1);
    sems->doctor_stats_mutex = semaphore_create(STR_SEM_DOCTOR_STATS_MUTEX, 1);
    sems->results_mutex = semaphore_create(STR_SEM_RESULTS_MUTEX, 1);
    sems->terminate_mutex = semaphore_create(STR_SEM_TERMINATE_MUTEX, 1);
}

void wakeup_processes(struct data_container* data, struct semaphores* sems) {
    for (int i = 0; i < data->n_patients; i++) {
        produce_end(sems->main_patient);
    }
    for (int i = 0; i < data->n_receptionists; i++) {
        produce_end(sems->patient_receptionist);
    }
    for (int i = 0; i < data->n_doctors; i++) {
        produce_end(sems->receptionist_doctor);
    }
}

void destroy_semaphores(struct semaphores* sems) {
    // Destruir prodcon main_patient
    semaphore_destroy(STR_SEM_MAIN_PATIENT_FULL, sems->main_patient->full);
    semaphore_destroy(STR_SEM_MAIN_PATIENT_EMPTY, sems->main_patient->empty);
    semaphore_destroy(STR_SEM_MAIN_PATIENT_MUTEX, sems->main_patient->mutex);

    // Destruir prodcon patient_receptionist
    semaphore_destroy(STR_SEM_PATIENT_RECEPT_FULL, sems->patient_receptionist->full);
    semaphore_destroy(STR_SEM_PATIENT_RECEPT_EMPTY, sems->patient_receptionist->empty);
    semaphore_destroy(STR_SEM_PATIENT_RECEPT_MUTEX, sems->patient_receptionist->mutex);

    // Destruir prodcon receptionist_doctor
    semaphore_destroy(STR_SEM_RECEPT_DOCTOR_FULL, sems->receptionist_doctor->full);
    semaphore_destroy(STR_SEM_RECEPT_DOCTOR_EMPTY, sems->receptionist_doctor->empty);
    semaphore_destroy(STR_SEM_RECEPT_DOCTOR_MUTEX, sems->receptionist_doctor->mutex);

    // Destruir semaphores mutex
    semaphore_destroy(STR_SEM_PATIENT_STATS_MUTEX, sems->patient_stats_mutex);
    semaphore_destroy(STR_SEM_RECEPT_STATS_MUTEX, sems->receptionist_stats_mutex);
    semaphore_destroy(STR_SEM_DOCTOR_STATS_MUTEX, sems->doctor_stats_mutex);
    semaphore_destroy(STR_SEM_RESULTS_MUTEX, sems->results_mutex);
    semaphore_destroy(STR_SEM_TERMINATE_MUTEX, sems->terminate_mutex);
}