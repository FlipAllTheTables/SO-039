#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "../include/memory.h"

/* Função que reserva uma zona de memória partilhada com tamanho indicado
* por size e nome name, preenche essa zona de memória com o valor 0, e 
* retorna um apontador para a mesma. Pode concatenar o resultado da função
* getuid() a name, para tornar o nome único para o processo.
*/
void* create_shared_memory(char* name, int size) {
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

/* Função que reserva uma zona de memória dinâmica com tamanho indicado
* por size, preenche essa zona de memória com o valor 0, e retorna um 
* apontador para a mesma.
*/
void* allocate_dynamic_memory(int size) {
    void* dyn_memory = calloc(1, size);
    if (dyn_memory == NULL) {
        puts("Erro em alocação de memória.");
        exit(1);
    }
    return calloc(1, size);
}

/* Função que liberta uma zona de memória partilhada previamente reservada.
*/
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

/* Função que liberta uma zona de memória dinâmica previamente reservada.
*/
void deallocate_dynamic_memory(void* ptr) {
    free(ptr);
}

/* Função que escreve uma admissão no buffer de memória partilhada entre a Main
* e os pacientes. A admissão deve ser escrita numa posição livre do buffer, 
* tendo em conta o tipo de buffer e as regras de escrita em buffers desse tipo.
* Se não houver nenhuma posição livre, não escreve nada.
*/
void write_main_patient_buffer(struct circular_buffer* buffer, int buffer_size, struct admission* ad) {
    // Verificar que buffer não está cheio
    if ((buffer->ptrs->in + 1) % buffer_size != buffer->ptrs->out) {
        buffer->buffer[buffer->ptrs->in] = *ad;
        buffer->ptrs->in++;
    }
}

/* Função que escreve uma admissão no buffer de memória partilhada entre os pacientes
* e os rececionistas. A admissão deve ser escrita numa posição livre do buffer, 
* tendo em conta o tipo de buffer e as regras de escrita em buffers desse tipo.
* Se não houver nenhuma posição livre, não escreve nada.
*/
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

/* Função que escreve uma admissão no buffer de memória partilhada entre os rececionistas
* e os médicos. A admissão deve ser escrita numa posição livre do buffer, 
* tendo em conta o tipo de buffer e as regras de escrita em buffers desse tipo.
* Se não houver nenhuma posição livre, não escreve nada.
*/
void write_receptionist_doctor_buffer(struct circular_buffer* buffer, int buffer_size, struct admission* ad) {
    // Verificar que buffer não está cheio
    if ((buffer->ptrs->in + 1) % buffer_size != buffer->ptrs->out) {
        buffer->buffer[buffer->ptrs->in] = *ad;
        buffer->ptrs->in++;
    }
}

/* Função que lê uma admissão do buffer de memória partilhada entre a Main
* e os pacientes, se houver alguma disponível para ler que seja direcionada ao paciente especificado.
* A leitura deve ser feita tendo em conta o tipo de buffer e as regras de leitura em buffers desse tipo.
* Se não houver nenhuma admissão disponível, afeta ad->id com o valor -1.
*/
void read_main_patient_buffer(struct circular_buffer* buffer, int patient_id, int buffer_size, struct admission* ad) {
    // Verficar que buffer não está vazio
    if (buffer->ptrs->in != buffer->ptrs->out) {
        *ad = buffer->buffer[buffer->ptrs->out];
        buffer->ptrs->out++;
    } else {
        // buffer cheio, admission ID definido como -1
        ad->id = -1;
    }
}

/* Função que lê uma admissão do buffer de memória partilhada entre os pacientes e rececionistas,
* se houver alguma disponível para ler (qualquer rececionista pode ler qualquer admissão).
* A leitura deve ser feita tendo em conta o tipo de buffer e as regras de leitura em buffers desse tipo.
* Se não houver nenhuma admissão disponível, afeta ad->id com o valor -1.
*/
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

/* Função que lê uma admissão do buffer de memória partilhada entre os rececionistas e os médicos,
* se houver alguma disponível para ler dirigida ao médico especificado. A leitura deve
* ser feita tendo em conta o tipo de buffer e as regras de leitura em buffers desse tipo. Se não houver
* nenhuma admissão disponível, afeta ad->id com o valor -1.
*/
void read_receptionist_doctor_buffer(struct circular_buffer* buffer, int doctor_id, int buffer_size, struct admission* ad) {
    // Verificar que buffer não está vazio
    if (buffer->ptrs->in != buffer->ptrs->out) {
        *ad = buffer->buffer[buffer->ptrs->out];
        buffer->ptrs->out++;
    } else {
        // buffer cheio, admission ID definido como -1
        ad->id = -1;
    }
}