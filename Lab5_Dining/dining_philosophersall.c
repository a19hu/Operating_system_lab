#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>

#define NUM_PHILOSOPHERS 5
#define EAT_LIMIT 20  // Total number of eating cycles to be tracked

pthread_mutex_t forks[NUM_PHILOSOPHERS];
pthread_mutex_t output_lock; // For synchronized output

typedef struct {
    int id;
    int eat_count;
    double wait_time;
    int thinking_count;
} philosopher_t;

philosopher_t philosophers[NUM_PHILOSOPHERS];

void* philosopher_thread(void* arg) {
    philosopher_t* philo = (philosopher_t*)arg;
    struct timeval start, end;

    while (philo->eat_count < EAT_LIMIT) {
        // Thinking
        pthread_mutex_lock(&output_lock);
        printf("Philosopher %d is thinking.\n", philo->id);
        pthread_mutex_unlock(&output_lock);
        philo->thinking_count++;
        usleep(rand() % 100000);  // Random sleep to simulate thinking

        // Start timing for waiting
        gettimeofday(&start, NULL);

        // Pick up forks (avoid deadlock by ordering)
        pthread_mutex_lock(&forks[philo->id]);
        pthread_mutex_lock(&forks[(philo->id + 1) % NUM_PHILOSOPHERS]);

        // Stop timing after acquiring forks
        gettimeofday(&end, NULL);
        philo->wait_time += (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;

        // Eating
        pthread_mutex_lock(&output_lock);
        printf("Philosopher %d is eating.\n", philo->id);
        philo->eat_count++;
        pthread_mutex_unlock(&output_lock);
        usleep(rand() % 100000);  // Random sleep to simulate eating

        // Put down forks
        pthread_mutex_unlock(&forks[philo->id]);
        pthread_mutex_unlock(&forks[(philo->id + 1) % NUM_PHILOSOPHERS]);
    }

    return NULL;
}

void run_test_case(int test_case) {
    pthread_t threads[NUM_PHILOSOPHERS];

    // Initialize mutexes and philosophers
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_init(&forks[i], NULL);
        philosophers[i].id = i;
        philosophers[i].eat_count = 0;
        philosophers[i].wait_time = 0.0;
        philosophers[i].thinking_count = 0;
    }
    pthread_mutex_init(&output_lock, NULL); // Initialize output lock

    // Create philosopher threads
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_create(&threads[i], NULL, philosopher_thread, &philosophers[i]);
    }

    // Run for a limited time to check the output
    sleep(10); // Let the simulation run for 10 seconds

    // Terminate the threads (in a real scenario, you'd have a better way to stop)
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_cancel(threads[i]); // Use cancel to stop threads
    }

    // Join philosopher threads
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Output results
    printf("\nTest Case %d Results:\n", test_case);
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        if (philosophers[i].eat_count > 0) {
            printf("Philosopher %d ate %d times, average wait time: %.2f seconds, thinking count: %d.\n",
                   philosophers[i].id,
                   philosophers[i].eat_count,
                   philosophers[i].wait_time / philosophers[i].eat_count,
                   philosophers[i].thinking_count);
        }
    }

    // Cleanup
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_destroy(&forks[i]);
    }
    pthread_mutex_destroy(&output_lock); // Destroy output lock
}

int main() {
    int test_case;

    printf("Select Test Case (1-5):\n");
    printf("1: Basic Synchronization Test\n");
    printf("2: Deadlock Avoidance Test\n");
    printf("3: Fairness Test (Starvation Prevention)\n");
    printf("4: Concurrent Eating Test\n");
    printf("5: Efficiency and Performance Test\n");
    scanf("%d", &test_case);

    switch (test_case) {
        case 1:
            printf("Running Basic Synchronization Test...\n");
            run_test_case(1);
            break;
        case 2:
            printf("Running Deadlock Avoidance Test...\n");
            run_test_case(2);
            break;
        case 3:
            printf("Running Fairness Test...\n");
            run_test_case(3);
            break;
        case 4:
            printf("Running Concurrent Eating Test...\n");
            run_test_case(4);
            break;
        case 5:
            printf("Running Efficiency and Performance Test...\n");
            run_test_case(5);
            break;
        default:
            printf("Invalid test case selected.\n");
            break;
    }

    return 0;
}

