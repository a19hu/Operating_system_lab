#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define NUM_PHILOSOPHERS 5
#define SIMULATION_TIME 5

pthread_mutex_t forks[NUM_PHILOSOPHERS];
pthread_t philosophers[NUM_PHILOSOPHERS];
int eat_count[NUM_PHILOSOPHERS] = {0};
double total_wait_time[NUM_PHILOSOPHERS] = {0};
int running = 1;  // Flag to stop simulation

void think(int id) {
    printf("Philosopher %d is thinking.\n", id);
    usleep(rand() % 1000000);  // Simulate time spent thinking
}

void eat(int id) {
    printf("Philosopher %d is eating.\n", id);
    usleep(rand() % 1000000);  // Simulate time spent eating
    eat_count[id]++;
    printf("Philosopher %d has finished eating.\n", id);  // Added print statement
}

void* philosopher(void* arg) {
    int id = *(int*)arg;
    int left_fork = id;
    int right_fork = (id + 1) % NUM_PHILOSOPHERS;
    struct timespec start, end;

    while (running) {
        think(id);

        clock_gettime(CLOCK_MONOTONIC, &start);

        if (id % 2 == 0) {
            pthread_mutex_lock(&forks[left_fork]);
            pthread_mutex_lock(&forks[right_fork]);
        } else {
            pthread_mutex_lock(&forks[right_fork]);
            pthread_mutex_lock(&forks[left_fork]);
        }

        clock_gettime(CLOCK_MONOTONIC, &end);
        total_wait_time[id] += (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

        eat(id);

        pthread_mutex_unlock(&forks[left_fork]);
        pthread_mutex_unlock(&forks[right_fork]);
    }

    return NULL;
}

int main() {
    srand(time(NULL));
    int philosopher_ids[NUM_PHILOSOPHERS];

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_init(&forks[i], NULL);
    }

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        philosopher_ids[i] = i;
        pthread_create(&philosophers[i], NULL, philosopher, &philosopher_ids[i]);
    }

    sleep(SIMULATION_TIME);

    running = 0;  // Stop the philosophers

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_join(philosophers[i], NULL);  // Gracefully join threads
    }

    printf("\nSimulation Results:\n");
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        printf("Philosopher %d ate %d times, average wait time: %.2f seconds.\n", 
               i, eat_count[i], total_wait_time[i] / eat_count[i]);
    }

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_destroy(&forks[i]);
    }

    return 0;
}
