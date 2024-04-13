#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#include "process.h"
#include "memory.h"
#include "main.h"
#include "patient.h"
#include "receptionist.h"
#include "doctor.h"

int launch_patient(int patient_id, struct data_container* data, struct communication* comm) {
    int pd = fork();
    if (pd == -1) { // Ocorreu erro
        puts("Erro em fork de criação de paciente");
        exit(1);
    } else if (pd == 0) { // Processo filho
        int patient_res = execute_patient(patient_id, data, comm);
        exit(patient_res);
    } else { // Processo pai
        return pd;
    }
}

int launch_receptionist(int receptionist_id, struct data_container* data, struct communication* comm) {
    int pd = fork();
    if (pd == -1) { // Ocorreu erro
        puts("Erro em fork de criação de rececionista");
        exit(1);
    } else if (pd == 0) { // Processo filho
        int receptionist_res = execute_receptionist(receptionist_id, data, comm);
        exit(receptionist_res);
    } else { // Processo pai
        return pd;
    }
}

int launch_doctor(int doctor_id, struct data_container* data, struct communication* comm) {
    int pd = fork();
    if (pd == -1) { // Ocorreu erro
        puts("Erro em fork de criação de médico");
        exit(1);
    } else if (pd == 0) { // Processo filho
        int doctor_res = execute_doctor(doctor_id, data, comm);
        exit(doctor_res);
    } else { // Processo pai
        return pd;
    }
}

int wait_process(int process_id) {
    int result;
    waitpid(process_id, &result, 0);
    if (WIFEXITED(result)) { // Verificar se filho terminou de forma normal
        return WEXITSTATUS(result); // Devolver 8 bits menos significativos do valor de retorno do processo
    }
    return -1;
}