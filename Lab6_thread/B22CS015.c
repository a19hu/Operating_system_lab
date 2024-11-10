#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

int sum = 0; // Global variable to store the sum

// Thread function that calculates the sum of two numbers
void *func_sum(void *arg) {
    int *nums = (int *)arg;   // Cast the argument to an integer pointer
    sum = nums[0] + nums[1];  
    pthread_exit(&sum);       // Exit the thread and return the sum
}

int main() {
    int num[2];               // Array to store two input numbers
    pthread_t T1;             // Thread identifier

    
    printf("Enter two numbers:\n");
    scanf("%d", &num[0]);   
    scanf("%d", &num[1]);    

   
    printf("A: %d \t B: %d\n", num[0], num[1]);

    // Create a new thread that runs the 'func_sum' function
    // Pass the 'num' array as an argument to the thread
    pthread_create(&T1, NULL, func_sum, (void*)num);

    // Wait for the thread to complete and retrieve the result (sum)
    int* ptr;
    pthread_join(T1, (void**)&ptr);  // 'ptr' points to the sum returned by the thread

    // Print the calculated sum
    printf("Sum: %d\n", *ptr);

    return 0;
}
