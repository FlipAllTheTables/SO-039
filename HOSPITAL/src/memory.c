#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "memory.h"

void* create_shared_memory(char* name, int size) {
    int shmem = shm_open(name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    int ftrunc = ftruncate(shmem, size);
    return mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, shmem, 0); 
}

void* allocate_dynamic_memory(int size) {
    return calloc(1, size);
}

void destroy_shared_memory(char* name, void* ptr, int size) {
    if (munmap(ptr, size) == -1) {
        printf("Erro com libertação de shared memory.");
        exit(1);
    }
    if (shm_unlink(name) == -1) {
        printf("Erro com libertação de nome de shared memory.");
        exit(2);
    }
}

void deallocate_dynamic_memory(void* ptr) {
    free(ptr);
}

void write_main_patient_buffer(struct circular_buffer* buffer, int buffer_size, struct admission* ad) {

}

void write_patient_receptionist_buffer(struct rnd_access_buffer* buffer, int buffer_size, struct admission* ad) {

}

void write_receptionist_doctor_buffer(struct circular_buffer* buffer, int buffer_size, struct admission* ad) {

}

void read_main_patient_buffer(struct circular_buffer* buffer, int patient_id, int buffer_size, struct admission* ad) {

}

void read_patient_receptionist_buffer(struct rnd_access_buffer* buffer, int buffer_size, struct admission* ad) {

}

void read_receptionist_doctor_buffer(struct circular_buffer* buffer, int doctor_id, int buffer_size, struct admission* ad) {

}