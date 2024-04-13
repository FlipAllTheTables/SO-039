#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "memory.h"

void* create_shared_memory(char* name, int size) {
    // TODO usar getuid()

    int shmem = shm_open(name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (shmem == -1) {
        puts("Erro na criação de zona de memória partilhada.");
        exit(1);
    }
    int ftrunc = ftruncate(shmem, size);
    if (ftrunc == -1) {
        puts("Erro em truncação da zona de memória partilhada.");
        exit(1);
    }
    void* mateamento = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, shmem, 0);
    if (mateamento == (void*) -1) {
        puts("Erro em mateamento de zona de memória partilhada.");
        exit(1);
    }
    return mateamento;
}

void* allocate_dynamic_memory(int size) {
    void* dyn_memory = calloc(1, size);
    if (dyn_memory == NULL) {
        puts("Erro em alocação de memória.");
        exit(1);
    }
    return calloc(1, size);
}

void destroy_shared_memory(char* name, void* ptr, int size) {
    if (munmap(ptr, size) == -1) {
        puts("Erro com libertação de shared memory.");
        exit(1);
    }
    if (shm_unlink(name) == -1) {
        puts("Erro com libertação de nome de shared memory.");
        exit(1);
    }
}

void deallocate_dynamic_memory(void* ptr) {
    free(ptr);
}

void write_main_patient_buffer(struct circular_buffer* buffer, int buffer_size, struct admission* ad) {
    // Verificar que buffer não está cheio
    if ((buffer->ptrs->in + 1) % buffer_size != buffer->ptrs->out) {
        buffer->buffer[buffer->ptrs->in] = *ad;
        buffer->ptrs->in++;
    }
}

void write_patient_receptionist_buffer(struct rnd_access_buffer* buffer, int buffer_size, struct admission* ad) {
    // Escrever no primeiro valor vazio, se houver um valor vazio
    for (int i = 0; i < buffer_size; i++) {
        if (buffer->ptrs[i] == 0) {
            buffer->buffer[i] = *ad;
            buffer->ptrs[i] == 1;
            return;
        }
    }
}

void write_receptionist_doctor_buffer(struct circular_buffer* buffer, int buffer_size, struct admission* ad) {
    // Verificar que buffer não está cheio
    if ((buffer->ptrs->in + 1) % buffer_size != buffer->ptrs->out) {
        buffer->buffer[buffer->ptrs->in] = *ad;
        buffer->ptrs->in++;
    }
}

void read_main_patient_buffer(struct circular_buffer* buffer, int patient_id, int buffer_size, struct admission* ad) {
    if (buffer->ptrs->in != buffer->ptrs->out) { // Verificar que buffer circular não está vazio
        if (buffer->buffer[buffer->ptrs->out].requesting_patient == patient_id) { // Verificar que admissão guardada é para o paciente especificado
            *ad = buffer->buffer[buffer->ptrs->out];
            buffer->ptrs->out = (buffer->ptrs->out + 1) % buffer_size;
            return;
        }
    }
    ad->id = -1;
}

void read_patient_receptionist_buffer(struct rnd_access_buffer* buffer, int buffer_size, struct admission* ad) {
    // Percorrer buffer até encontrar posição com admissão
    for (int i = 0; i < buffer_size; i++) {
        if (buffer->ptrs[i] == 1) {
            *ad = buffer->buffer[i];
            buffer->ptrs[i] == 0;
            return;
        }
    }
    // Não se encontrou uma admissão no buffer
    ad->id = -1;
}

void read_receptionist_doctor_buffer(struct circular_buffer* buffer, int doctor_id, int buffer_size, struct admission* ad) {
    if (buffer->ptrs->in != buffer->ptrs->out) { // Verificar que buffer não está vazio
        if (buffer->buffer[buffer->ptrs->out].requested_doctor == doctor_id) { // Verificar que admissão guardada é para o médico especificado
            *ad = buffer->buffer[buffer->ptrs->out];
            buffer->ptrs->out = (buffer->ptrs->out + 1) % buffer_size;
            return;
        }
    }
    ad->id = -1;
}