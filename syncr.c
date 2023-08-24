

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 5
#define NUM_PRODUCERS 2
#define NUM_CONSUMERS 2
#define NUM_READERS 3
#define NUM_WRITERS 2
#define NUM_PHILOSOPHERS 5

// Producer-Consumer Problem
int buffer[BUFFER_SIZE];
int in = 0;
int out = 0;

sem_t emptySlots;    // Semaphore to track empty slots in the buffer
sem_t filledSlots;   // Semaphore to track filled slots in the buffer
sem_t bufferMutex;   // Semaphore for mutual exclusion access to the buffer

void *producer(void *producerId) {
    int id = *(int *)producerId;
    int item = 0;

    while (1) {
        // Produce item
        item++;

        sem_wait(&emptySlots); // Wait for an empty slot in the buffer
        sem_wait(&bufferMutex); // Obtain exclusive access to the buffer

        // Add item to the buffer
        buffer[in] = item;
        printf("Producer %d produced item %d\n", id, item);
        in = (in + 1) % BUFFER_SIZE;

        sem_post(&bufferMutex); // Release exclusive access to the buffer
        sem_post(&filledSlots); // Signal that a slot in the buffer is filled

        // Sleep for a random period
        usleep(rand() % 1000000);
    }
}

void *consumer(void *consumerId) {
    int id = *(int *)consumerId;
    int item;

    while (1) {
        sem_wait(&filledSlots); // Wait for a filled slot in the buffer
        sem_wait(&bufferMutex); // Obtain exclusive access to the buffer

        // Consume item from the buffer
        item = buffer[out];
        printf("Consumer %d consumed item %d\n", id, item);
        out = (out + 1) % BUFFER_SIZE;

        sem_post(&bufferMutex); // Release exclusive access to the buffer
        sem_post(&emptySlots); // Signal that a slot in the buffer is empty

        // Sleep for a random period
        usleep(rand() % 1000000);
    }
}

// Reader-Writer Problem
int sharedData = 0;
int readerCount = 0;
sem_t mutex;       // Semaphore for mutual exclusion access to sharedData
sem_t rwMutex;     // Semaphore for mutual exclusion access to readerCount

void *reader(void *readerId) {
    int id = *(int *)readerId;

    while (1) {
        sem_wait(&mutex); // Obtain exclusive access to sharedData
        sem_wait(&rwMutex); // Obtain exclusive access to readerCount

        readerCount++;
        if (readerCount == 1) {
            sem_wait(&rwMutex); // Wait for exclusive access to sharedData
        }

        sem_post(&mutex); // Release exclusive access to sharedData
        sem_post(&rwMutex); // Release exclusive access to readerCount

        // Read data
        printf("Reader %d read shared data: %d\n", id, sharedData);

        sem_wait(&rwMutex); // Obtain exclusive access to readerCount

        readerCount--;
        if (readerCount == 0) {
            sem_post(&rwMutex); // Signal writer that it can access sharedData
        }

        sem_post(&rwMutex); // Release exclusive access to readerCount

        // Sleep for a random period
        usleep(rand() % 1000000);
    }
}

void *writer(void *writerId) {
    int id = *(int *)writerId;

    while (1) {
        sem_wait(&rwMutex); // Wait for exclusive access to sharedData

        // Write data
        sharedData++;
        printf("Writer %d wrote shared data: %d\n", id, sharedData);

        sem_post(&rwMutex); // Signal reader(s) that they can access sharedData

        // Sleep for a random period
        usleep(rand() % 1000000);
    }
}

// Dining Philosophers Problem
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
    // Producer-Consumer Problem
    sem_init(&emptySlots, 0, BUFFER_SIZE); // Initialize emptySlots semaphore with buffer size
    sem_init(&filledSlots, 0, 0); // Initialize filledSlots semaphore with 0
    sem_init(&bufferMutex, 0, 1); // Initialize bufferMutex semaphore with 1

    pthread_t producers[NUM_PRODUCERS];
    pthread_t consumers[NUM_CONSUMERS];

    int producerIds[NUM_PRODUCERS];
    int consumerIds[NUM_CONSUMERS];

    for (int i = 0; i < NUM_PRODUCERS; i++) {
        producerIds[i] = i + 1;
        pthread_create(&producers[i], NULL, producer, (void *)&producerIds[i]);
    }

    for (int i = 0; i < NUM_CONSUMERS; i++) {
        consumerIds[i] = i + 1;
        pthread_create(&consumers[i], NULL, consumer, (void *)&consumerIds[i]);
    }

    // Reader-Writer Problem
    sem_init(&mutex, 0, 1); // Initialize mutex semaphore with 1
    sem_init(&rwMutex, 0, 1); // Initialize rwMutex semaphore with 1

    pthread_t readers[NUM_READERS];
    pthread_t writers[NUM_WRITERS];

    int readerIds[NUM_READERS];
    int writerIds[NUM_WRITERS];

    for (int i = 0; i < NUM_READERS; i++) {
        readerIds[i] = i + 1;
        pthread_create(&readers[i], NULL, reader, (void *)&readerIds[i]);
    }

    for (int i = 0; i < NUM_WRITERS; i++) {
        writerIds[i] = i + 1;
        pthread_create(&writers[i], NULL, writer, (void *)&writerIds[i]);
    }

    // Dining Philosophers Problem
    sem_init(&philMutex, 0, 1); // Initialize philMutex semaphore with 1

    pthread_t philosophers[NUM_PHILOSOPHERS];

    int philosopherIds[NUM_PHILOSOPHERS];

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        philosopherIds[i] = i + 1;
        sem_init(&philSem[i], 0, 0); // Initialize philSemaphores with 0
        pthread_create(&philosophers[i], NULL, philosopher, (void *)&philosopherIds[i]);
    }

    // Wait for threads to finish
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(producers[i], NULL);
    }

    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_join(consumers[i], NULL);
    }

    for (int i = 0; i < NUM_READERS; i++) {
        pthread_join(readers[i], NULL);
    }

    for (int i = 0; i < NUM_WRITERS; i++) {
        pthread_join(writers[i], NULL);
    }

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_join(philosophers[i], NULL);
    }

    // Cleanup semaphores
    sem_destroy(&emptySlots);
    sem_destroy(&filledSlots);
    sem_destroy(&bufferMutex);

    sem_destroy(&mutex);
    sem_destroy(&rwMutex);

    sem_destroy(&philMutex);
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        sem_destroy(&philSem[i]);
    }

    return 0;
}

