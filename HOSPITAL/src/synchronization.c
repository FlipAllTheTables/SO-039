/*  Grupo 039
 * Rafael Ribeiro   - 58193
 * Pedro Duque      - 52753
 * Francisco Santos - 59871
*/ 

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>

#include "synchronization.h"

sem_t * semaphore_create(char* name, int value) {
    sem_t *sem;
    sem = sem_open(name, O_CREAT, 0xFFFFFFFF, value);
    if (sem == SEM_FAILED) {
        puts("Erro criação de semaphore.");
        exit(1);
    }
    return sem;
}

void semaphore_destroy(char* name, sem_t* semaphore) {
    if (sem_close(semaphore) == -1) {
        puts("Erro sem_close.");
        exit(1);
    }
    if (sem_destroy(semaphore) == -1) {
        puts("Erro sem_destroy.");
        exit(1);
    }
}

void produce_begin(struct prodcons* pc) {
    semaphore_lock(pc->empty);
    semaphore_lock(pc->mutex);
}

void produce_end(struct prodcons* pc) {
    semaphore_unlock(pc->mutex);
    semaphore_unlock(pc->full);
}

void consume_begin(struct prodcons* pc) {
    semaphore_lock(pc->full);
    semaphore_lock(pc->mutex);
}

void consume_end(struct prodcons* pc) {
    semaphore_unlock(pc->mutex);
    semaphore_unlock(pc->empty);
}

void semaphore_lock(sem_t* sem) {
    if (sem_wait(sem) == -1) {
        puts("Erro sem_wait.");
        exit(1);
    }
}

void semaphore_unlock(sem_t* sem) {
    if (sem_post(sem) == -1) {
        puts("Erro sem_post.");
        exit(1);
    }
}