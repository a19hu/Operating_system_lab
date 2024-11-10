#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

// Function that will be executed by the thread
void *func_for_thread(void *arg);

int main() {
    pthread_t t1;
    
    // Create a new thread that runs func_for_thread
    pthread_create(&t1, NULL, func_for_thread, NULL);
    
    // Wait for the thread to finish
 //   pthread_join(t1, NULL);

    printf("Entering main thread...\n");

    // Main thread loop
    for (int j = 0; j < 5; j++) {
        printf("Main: %d\n", j + 1);
        sleep(1);
    }

    return 0; // Good practice to return from main
}

// Thread function
void *func_for_thread(void *arg) {
    printf("Entering thread...\n");

    // Thread loop
    for (int i = 20; i < 25; i++) {
        printf("Thread: %d\n", i + 1);
        sleep(1);
    }

    return NULL; // Return NULL from the thread function
}

