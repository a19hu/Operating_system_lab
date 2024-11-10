// Receiver.c
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <stdlib.h>

#define SHM_SIZE 2048

typedef struct {
    int ready;
    char message[SHM_SIZE];
} SharedData;

typedef struct {
    char word[100];
    int length;
    int index;
} Word;

int compare_ascending(const void* a, const void* b) {
    Word* wA = (Word*)a;
    Word* wB = (Word*)b;
    if (wA->length != wB->length)
        return wA->length - wB->length;
    else
        return wA->word[0] - wB->word[0];
}

int compare_descending(const void* a, const void* b) {
    Word* wA = (Word*)a;
    Word* wB = (Word*)b;
    if (wB->length != wA->length)
        return wB->length - wA->length;
    else
        return wB->word[0] - wA->word[0];
}

int main() {
    key_t key = ftok("shmfile", 65);
    int shmid = shmget(key, sizeof(SharedData), 0666 | IPC_CREAT);
    SharedData* data = (SharedData*) shmat(shmid, (void*)0, 0);

    while (data->ready == 0);
    char input[SHM_SIZE];
    strcpy(input, data->message);
    data->ready = 0;
    shmdt(data);

    int len = strlen(input);
    if (len >= 2 && input[0] == '"' && input[len - 1] == '"') {
        memmove(input, input + 1, len - 2);
        input[len - 2] = '\0';
    }

    char order[20];
    printf("Enter sorting order (ascending/descending): ");
    scanf("%s", order);
    int ascending = (strcmp(order, "descending") != 0);

    Word words[100];
    int wordCount = 0;
    char* token;
    char* rest = input;
    int index = 0;

    while ((token = strtok_r(rest, " \t\n", &rest))) {
        int tlen = strlen(token);
        if (tlen >= 2 && token[0] == '"' && token[tlen - 1] == '"') {
            memmove(token, token + 1, tlen - 2);
            token[tlen - 2] = '\0';
        }
        strcpy(words[wordCount].word, token);
        words[wordCount].length = strlen(token);
        words[wordCount].index = index++;
        wordCount++;
    }

    char largestWord[100] = "";
    int maxLen = 0;
    int maxIdx = -1;
    for (int i = 0; i < wordCount; i++) {
        if (words[i].length > maxLen || (words[i].length == maxLen && words[i].index > maxIdx)) {
            maxLen = words[i].length;
            maxIdx = words[i].index;
            strcpy(largestWord, words[i].word);
        }
    }

    if (ascending)
        qsort(words, wordCount, sizeof(Word), compare_ascending);
    else
        qsort(words, wordCount, sizeof(Word), compare_descending);

    printf("Largest Word: \"%s\"\n", largestWord);
    printf("Words sorted by length (%s): ", ascending ? "Ascending" : "Descending");
    for (int i = 0; i < wordCount; i++) {
        printf("%s", words[i].word);
        if (i != wordCount - 1) printf(" ");
    }
    printf("\n");

    return 0;
}