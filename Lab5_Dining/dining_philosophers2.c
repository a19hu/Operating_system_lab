#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

#define NUM_PHILOSOPHERS 5

pthread_mutex_t forks[NUM_PHILOSOPHERS];
pthread_mutex_t output_lock; // For synchronized output
pthread_mutex_t eat_lock; // To control maximum simultaneous eaters
int eat_count = 0; // Count how many philosophers have eaten

void* philosopher(void* id) {
    int philosopher_id = *(int*)id;

    while (1) {
        // Thinking
        pthread_mutex_lock(&output_lock);
        printf("Philosopher %d is thinking.\n", philosopher_id);
        pthread_mutex_unlock(&output_lock);
        usleep(rand() % 100000);  // Random sleep to simulate thinking

        // Try to enter the eating section
        pthread_mutex_lock(&eat_lock);
        if (eat_count < NUM_PHILOSOPHERS - 1) { // Limit to prevent deadlock
            // Pick up forks (avoid deadlock by ordering)
            if (philosopher_id % 2 == 0) {
                pthread_mutex_lock(&forks[philosopher_id]);
                pthread_mutex_lock(&forks[(philosopher_id + 1) % NUM_PHILOSOPHERS]);
            } else {
                pthread_mutex_lock(&forks[(philosopher_id + 1) % NUM_PHILOSOPHERS]);
                pthread_mutex_lock(&forks[philosopher_id]);
            }

            // Eating
            pthread_mutex_lock(&output_lock);
            printf("Philosopher %d is eating.\n", philosopher_id);
            eat_count++;
            pthread_mutex_unlock(&output_lock);
            usleep(rand() % 100000);  // Random sleep to simulate eating

            // Put down forks
            pthread_mutex_unlock(&forks[philosopher_id]);
            pthread_mutex_unlock(&forks[(philosopher_id + 1) % NUM_PHILOSOPHERS]);

            pthread_mutex_lock(&output_lock);
            printf("Philosopher %d has finished eating.\n", philosopher_id);
            pthread_mutex_unlock(&output_lock);
        }
        pthread_mutex_unlock(&eat_lock);

        // Reset eat count after each cycle
        if (eat_count == NUM_PHILOSOPHERS) {
            pthread_mutex_lock(&output_lock);
            printf("All philosophers have eaten at least once.\n");
            eat_count = 0; // Reset for the next cycle
            pthread_mutex_unlock(&output_lock);
        }
    }

    return NULL;
}

int main() {
    pthread_t philosophers[NUM_PHILOSOPHERS];
    int ids[NUM_PHILOSOPHERS];

    // Initialize mutexes
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_init(&forks[i], NULL);
        ids[i] = i;
    }
    pthread_mutex_init(&output_lock, NULL); // Initialize output lock
    pthread_mutex_init(&eat_lock, NULL); // Initialize eat control lock

    // Create philosopher threads
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_create(&philosophers[i], NULL, philosopher, &ids[i]);
    }

    // Run for a limited time to check the output
    sleep(10); // Let the simulation run for 10 seconds

    // Terminate the threads (in a real scenario, you'd have a better way to stop)
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_cancel(philosophers[i]); // Use cancel to stop threads
    }

    // Join philosopher threads
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_join(philosophers[i], NULL);
    }

    // Cleanup
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_destroy(&forks[i]);
    }
    pthread_mutex_destroy(&output_lock); // Destroy output lock
    pthread_mutex_destroy(&eat_lock); // Destroy eat control lock

    return 0;
}

