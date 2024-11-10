#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

#define SHM_SIZE 2048

typedef struct {
    int ready;
    char message[SHM_SIZE];
} SharedData;

int main() {
    key_t key = ftok("shmfile", 65);
    int shmid = shmget(key, sizeof(SharedData), 0666 | IPC_CREAT);
    SharedData* data = (SharedData*) shmat(shmid, (void*)0, 0);
    data->ready = 0;
    printf("Enter a string: ");
    fgets(data->message, SHM_SIZE, stdin);
    data->message[strcspn(data->message, "\n")] = '\0';
    data->ready = 1;
    shmdt(data);
    return 0;
}