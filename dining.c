#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#define NUM_PHILOSOPHERS 5

enum { THINKING, HUNGRY, EATING } state[5];
sem_t philMutex;
sem_t philSem[5];

void *philosopher(void *philosopherId) {
    int id = *(int *)philosopherId;

    while (1) {
        printf("Philosopher %d is thinking\n", id);

        // Start thinking
        usleep(rand() % 1000000);

        sem_wait(&philMutex); // Obtain exclusive access to state

        state[id] = HUNGRY;
        printf("Philosopher %d is hungry\n", id);

        // Try to acquire the chopsticks
        test(id);

        sem_post(&philMutex); // Release exclusive access to state
        sem_wait(&philSem[id]); // Wait for chopsticks

        printf("Philosopher %d is eating\n", id);

        // Start eating
        usleep(rand() % 1000000);

        sem_post(&philSem[id]); // Put down chopsticks

        printf("Philosopher %d finished eating\n", id);
    }
}

void test(int id) {
    if (state[id] == HUNGRY && state[(id + 1) % 5] != EATING && state[(id + 4) % 5] != EATING) {
        state[id] = EATING;
        sem_post(&philSem[id]);
    }
}

int main() {

    // Dining Philosophers Problem
    sem_init(&philMutex, 0, 1); // Initialize philMutex semaphore with 1

    pthread_t philosophers[NUM_PHILOSOPHERS];

    int philosopherIds[NUM_PHILOSOPHERS];

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        philosopherIds[i] = i + 1;
        sem_init(&philSem[i], 0, 0); // Initialize philSemaphores with 0
        pthread_create(&philosophers[i], NULL, philosopher, (void *)&philosopherIds[i]);
    }

  

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_join(philosophers[i], NULL);
    }

    // Cleanup semaphores


   

    sem_destroy(&philMutex);
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        sem_destroy(&philSem[i]);
    }

    return 0;
}