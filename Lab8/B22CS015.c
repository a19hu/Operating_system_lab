#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

// Structure to hold pairs and sums
typedef struct {
    int pair1_a;
    int pair1_b;
    int pair2_a;
    int pair2_b;
    int sum1;
    int sum2;
    pthread_mutex_t mutex;
} shared_data;

// Thread argument structure
typedef struct {
    int a;
    int b;
    int sum_index; // 1 for sum1, 2 for sum2
    int shmid;
} thread_arg;

// Function to compute sum and store in shared memory
void* compute_sum(void* arg){
    thread_arg *targ = (thread_arg*) arg;

    // Attach to shared memory
    shared_data *data = (shared_data*) shmat(targ->shmid, NULL, 0);
    if(data == (void*) -1){
        perror("shmat failed");
        pthread_exit(NULL);
    }

    int sum = targ->a + targ->b;

    // Lock mutex before writing to shared memory
    pthread_mutex_lock(&(data->mutex));

    if(targ->sum_index == 1){
        data->sum1 = sum;
    }
    else{
        data->sum2 = sum;
    }

    // Unlock mutex after writing
    pthread_mutex_unlock(&(data->mutex));

    // Detach shared memory
    shmdt(data);

    pthread_exit(NULL);
}

int main(){
    // Create shared memory
    key_t key = IPC_PRIVATE;
    int shmid = shmget(key, sizeof(shared_data), IPC_CREAT | 0666);
    if(shmid < 0){
        perror("shmget failed");
        exit(1);
    }

    // Attach to shared memory
    shared_data *data = (shared_data*) shmat(shmid, NULL, 0);
    if(data == (void*) -1){
        perror("shmat failed");
        exit(1);
    }

    // Initialize shared data
    data->sum1 = 0;
    data->sum2 = 0;

    // Initialize mutex with shared attribute
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&(data->mutex), &attr);

    // Input: read the two pairs
    printf("Enter Pair 1 (two integers): ");
    scanf("%d %d", &(data->pair1_a), &(data->pair1_b));

    printf("Enter Pair 2 (two integers): ");
    scanf("%d %d", &(data->pair2_a), &(data->pair2_b));

    // Create thread arguments
    thread_arg t1_arg = {data->pair1_a, data->pair1_b, 1, shmid};
    thread_arg t2_arg = {data->pair2_a, data->pair2_b, 2, shmid};

    // Create threads
    pthread_t T1, T2;
    if(pthread_create(&T1, NULL, compute_sum, (void*)&t1_arg)){
        perror("pthread_create T1 failed");
        exit(1);
    }

    if(pthread_create(&T2, NULL, compute_sum, (void*)&t2_arg)){
        perror("pthread_create T2 failed");
        exit(1);
    }

    // Wait for threads to finish
    pthread_join(T1, NULL);
    pthread_join(T2, NULL);

    // Retrieve sums
    int sum1 = data->sum1;
    int sum2 = data->sum2;
    int total_sum = sum1 + sum2;

    printf("Sum from T1: %d\n", sum1);
    printf("Sum from T2: %d\n", sum2);
    printf("Total Sum: %d\n", total_sum);

    // Cleanup
    pthread_mutex_destroy(&(data->mutex));
    shmdt(data);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
